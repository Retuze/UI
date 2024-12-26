#include "widgets/text_view.h"

TextView::TextView(const std::string& text) : text(text) {}

void TextView::draw(Renderer* renderer) {
    // TODO: 绘制文本
}

bool TextView::handleEvent(const Event& event) {
    return false;  // TextView 默认不处理事件
}

void TextView::setText(const std::string& text) {
    this->text = text;
} 