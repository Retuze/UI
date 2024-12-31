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
    static const Color Black;  // RGB(0, 0, 0)
    static const Color White;  // RGB(255, 255, 255)
    // ... other colors ...
    
    uint8_t r, g, b, a;
    Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}
    
    uint32_t toARGB() const {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }
};

// 像素格式枚举
enum class PixelFormat {
    RGB565,    // 16位，嵌入式常用
    RGB888,    // 24位
    RGBA8888,  // 32位
    A8,        // 8位alpha通道，用于字体渲染
}; 