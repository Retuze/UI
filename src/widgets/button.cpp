#include "widgets/button.h"
#include "graphics/font_manager.h"

Button::Button(const std::string& text) : text(text) {}

void Button::draw(Renderer* renderer) {
    // 绘制背景
    Color bgColor = backgroundColor;
    if (isPressed) {
        bgColor = pressedColor;
    } else if (isHovered) {
        bgColor = hoverColor;
    }
    renderer->fillRect(bounds, bgColor);
    
    // 绘制文本
    auto& fontManager = FontManager::getInstance();
    
    // 计算文本位置（居中显示）
    // TODO: 需要获取文本尺寸来实现真正的居中
    int textX = bounds.x + (bounds.width - text.length() * 12) / 2;  // 简单估算文本宽度
    int textY = bounds.y + bounds.height / 2 + 8;  // 估算基线位置
    
    fontManager.renderText(renderer, text, textX, textY, textColor);
}

bool Button::onMousePress(const Event& event) {
    if (bounds.contains(event.position)) {
        isPressed = true;
        return true;
    }
    return false;
}

bool Button::onMouseRelease(const Event& event) {
    if (bounds.contains(event.position) && isPressed) {
        isPressed = false;
        if (onClick) {
            onClick();
        }
        return true;
    }
    isPressed = false;
    return false;
}

bool Button::onMouseMove(const Event& event) {
    if (bounds.contains(event.position)) {
        isHovered = true;
    } else {
        isHovered = false;
        isPressed = false;
    }
    return true;
}

void Button::setText(const std::string& text) {
    this->text = text;
}

void Button::setTextColor(const Color& color) {
    textColor = color;
}

void Button::setBackgroundColor(const Color& color) {
    backgroundColor = color;
}

void Button::setHoverColor(const Color& color) {
    hoverColor = color;
}

void Button::setPressedColor(const Color& color) {
    pressedColor = color;
}

void Button::setOnClickListener(std::function<void()> listener) {
    onClick = std::move(listener);
} 