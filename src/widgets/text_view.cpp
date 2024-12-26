#include "widgets/text_view.h"
#include "graphics/font_manager.h"

TextView::TextView(const std::string& text) : text(text) {}

void TextView::draw(Renderer* renderer) {
    auto& fontManager = FontManager::getInstance();
    
    // 计算文本位置（考虑padding）
    int x = bounds.x + padding.left;
    int y = bounds.y + padding.top + textSize; // 基线位置
    
    // 绘制文本
    fontManager.renderText(renderer, text, x, y, textColor);
}

void TextView::setText(const std::string& text) {
    this->text = text;
}

void TextView::setTextColor(const Color& color) {
    textColor = color;
}

void TextView::setTextSize(int size) {
    textSize = size;
    // 这里应该重新加载字体，暂时简化处理
}

void TextView::setPadding(int left, int top, int right, int bottom) {
    padding.left = left;
    padding.top = top;
    padding.right = right;
    padding.bottom = bottom;
} 