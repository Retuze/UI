#pragma once

struct LayoutParams {
    static constexpr int MATCH_PARENT = -1;
    static constexpr int WRAP_CONTENT = -2;
    
    int width = WRAP_CONTENT;
    int height = WRAP_CONTENT;
    int marginLeft = 0;
    int marginTop = 0;
    int marginRight = 0;
    int marginBottom = 0;
    float weight = 0.0f;
    
    LayoutParams() = default;
    LayoutParams(int w, int h) : width(w), height(h) {}
    
    // Fluent interface for setting margins
    LayoutParams& setMargins(int left, int top, int right, int bottom) {
        marginLeft = left;
        marginTop = top;
        marginRight = right;
        marginBottom = bottom;
        return *this;
    }
    
    LayoutParams& setMargin(int margin) {
        return setMargins(margin, margin, margin, margin);
    }
    
    LayoutParams& setWeight(float w) {
        weight = w;
        return *this;
    }
}; 