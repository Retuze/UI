#pragma once
#include <cstdint>

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Size {
    int width, height;
    Size(int w = 0, int h = 0) : width(w), height(h) {}
};

struct Rect {
    int x, y, width, height;
    Rect(int x = 0, int y = 0, int w = 0, int h = 0) 
        : x(x), y(y), width(w), height(h) {}
    
    bool contains(const Point& p) const {
        return p.x >= x && p.x < x + width &&
               p.y >= y && p.y < y + height;
    }
};

struct Color {
    uint8_t r, g, b, a;
    Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}
}; 

// 像素格式枚举
enum class PixelFormat {
    RGB565,    // 16位，嵌入式常用
    RGB888,    // 24位
    RGBA8888,  // 32位
    A8,        // 8位alpha通道，用于字体渲染
}; 