#pragma once
#include <algorithm>
#include <cstdint>

// Basic geometric structures
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
        
    bool isEmpty() const { return width <= 0 || height <= 0; }
    bool contains(int px, int py) const {
        return px >= x && px < x + width && 
               py >= y && py < y + height;
    }
    
    Rect intersect(const Rect& other) const {
        int l = std::max(x, other.x);
        int t = std::max(y, other.y);
        int r = std::min(x + width, other.x + other.width);
        int b = std::min(y + height, other.y + other.height);
        
        if (l >= r || t >= b) return Rect();
        return Rect(l, t, r - l, b - t);
    }
};
