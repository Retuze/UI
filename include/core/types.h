#pragma once
#include <algorithm>
#include <cstdint>
#include <windef.h>
#include <windows.h>

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Size {
    int width, height;
    Size(int w = 0, int h = 0) : width(w), height(h) {}
};

struct Rect {
    int x = 0, y = 0;
    int width = 0, height = 0;
    
    Rect() = default;
    Rect(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height) {}
        
    Rect intersect(const Rect& other) const {
        int l = std::max(x, other.x);
        int t = std::max(y, other.y);
        int r = std::min(x + width, other.x + other.width);
        int b = std::min(y + height, other.y + other.height);
        
        if (l >= r || t >= b) return Rect();
        return Rect(l, t, r - l, b - t);
    }
    
    bool isEmpty() const {
        return width <= 0 || height <= 0;
    }
};

class Bitmap {
public:
    Bitmap(int width, int height);
    ~Bitmap();
    
    void* getPixels() const { return pixels; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getStride() const { return stride; }
    
private:
    void* pixels = nullptr;
    int width = 0;
    int height = 0;
    int stride = 0;
};

struct Color {
    uint8_t r, g, b, a;
    
    Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}

    // 大端序 (网络字节序, 高位在前)
    uint32_t toBGRA8888BE() const {
        return (b << 24) | (g << 16) | (r << 8) | a;
    }

    uint32_t toRGBA8888BE() const {
        return (r << 24) | (g << 16) | (b << 8) | a;
    }

    // 小端序 (x86字节序, 低位在前)
    uint32_t toBGRA8888LE() const {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    uint32_t toRGBA8888LE() const {
        return (a << 24) | (b << 16) | (g << 8) | r;
    }

    // 24位色
    uint32_t toRGB888BE() const {
        return (r << 16) | (g << 8) | b;
    }

    // 16位色
    uint16_t toRGB565BE() const {
        return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    }

    // 8位alpha
    uint8_t toA8BE() const {
        return a;
    }

    // 24位色
    uint32_t toRGB888LE() const {
        return (b << 16) | (g << 8) | r;
    }

    // 16位色
    uint16_t toRGB565LE() const {
        return ((b >> 3) << 11) | ((g >> 2) << 5) | (r >> 3);
    }

    // 8位alpha
    uint8_t toA8LE() const {
        return a;
    }

    static const Color& White() { static const Color c{255, 255, 255}; return c; }
    static const Color& Black() { static const Color c{0, 0, 0}; return c; }
    static const Color& Gray() { static const Color c{128, 128, 128}; return c; }
    static const Color& LightGray() { static const Color c{192, 192, 192}; return c; }
    static const Color& DarkGray() { static const Color c{64, 64, 64}; return c; }
};

// 像素格式枚举
enum class PixelFormat {
    RGB565BE,     // 16位，嵌入式常用
    RGB888BE,     // 24位
    RGBA8888BE,   // 32位
    BGRA8888BE,   // 32位
    A8BE,         // 8位alpha通道，用于字体渲染
    RGB565LE,     // 16位，嵌入式常用
    RGB888LE,     // 24位
    RGBA8888LE,   // 32位，Windows GDI默认格式
    BGRA8888LE,   // 32位
    A8LE,         // 8位alpha通道，用于字体渲染
}; 