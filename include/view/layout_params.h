#pragma once

class LayoutParams {
public:
    static constexpr int MATCH_PARENT = -1;
    static constexpr int WRAP_CONTENT = -2;
    
    int width = WRAP_CONTENT;
    int height = WRAP_CONTENT;
    int marginLeft = 0;
    int marginTop = 0;
    int marginRight = 0;
    int marginBottom = 0;
    
    LayoutParams() = default;
    LayoutParams(int width, int height) : width(width), height(height) {}
    
    bool isMatchParent(int size) const { return size == MATCH_PARENT; }
    bool isWrapContent(int size) const { return size == WRAP_CONTENT; }
}; 