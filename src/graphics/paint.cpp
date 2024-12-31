#include "graphics/paint.h"

float Paint::measureText(const std::string& text) const {
    return text.length() * textSize * 0.6f; // Simple approximation
}

float Paint::getTextHeight() const {
    return textSize;
} 