#pragma once
#include "core/types.h"
#include "graphics/pixel.h"
#include <string>

class Paint {
public:
    enum Style {
        Fill,
        Stroke,
        StrokeAndFill
    };
    
    Paint() = default;
    
    void setColor(Color color) { this->color = color; }
    void setStyle(Style style) { this->style = style; }
    void setStrokeWidth(float width) { strokeWidth = width; }
    void setTextSize(float size) { textSize = size; }
    void setAlpha(uint8_t alpha) { this->alpha = alpha; }
    
    Color getColor() const { return color; }
    Style getStyle() const { return style; }
    float getStrokeWidth() const { return strokeWidth; }
    float getTextSize() const { return textSize; }
    uint8_t getAlpha() const { return alpha; }
    
    float measureText(const std::string& text) const;
    float getTextHeight() const;
    
private:
    Color color{0, 0, 0};
    Style style = Fill;
    float strokeWidth = 1.0f;
    float textSize = 12.0f;
    uint8_t alpha = 255;
}; 