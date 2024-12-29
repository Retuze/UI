#include "widgets/button.h"
#include "graphics/font_manager.h"
#include "graphics/dpi_manager.h"
#include "core/types.h"

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
    auto& dpiManager = DPIManager::getInstance();
    
    // 使用 DPI 感知的文本大小
    int scaledTextSize = dpiManager.scaleY(12);  // 基础文本大小
    int textX = bounds.x + (bounds.width - text.length() * scaledTextSize) / 2;
    int textY = bounds.y + bounds.height / 2 + dpiManager.scaleY(8);
    
    fontManager.renderText(renderer, text, textX, textY, textColor);
}

bool Button::onMousePress(const Event& event) {
    if (bounds.contains(event.position)) {
        isPressed = true;
        isHovered = true;
        Widget::setCapturedWidget(this);  // 设置为当前捕获事件的控件
        return true;
    }
    return false;
}

bool Button::onMouseRelease(const Event& event) {
    // 如果是当前捕获事件的控件，则处理释放事件
    if (Widget::getCapturedWidget() == this) {
        bool wasPressed = isPressed;
        isPressed = false;
        Widget::setCapturedWidget(nullptr);  // 清除捕获
        
        if (bounds.contains(event.position)) {
            if (wasPressed && onClick) {
                onClick();
            }
            return true;
        }
        
        isHovered = false;
        return true;  // 即使在按钮外释放，也要消费这个事件
    }
    return false;
}

bool Button::onMouseMove(const Event& event) {
    if (Widget::getCapturedWidget() == this) {
        // 如果是捕获的控件，根据位置更新状态
        isHovered = bounds.contains(event.position);
        return true;
    }
    
    // 非捕获状态下的正常处理
    isHovered = bounds.contains(event.position);
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