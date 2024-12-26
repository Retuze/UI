#include "widgets/text_view.h"
#include "graphics/font_manager.h"

TextView::TextView(const std::string& text) : text(text) {}

void TextView::draw(Renderer* renderer) {
    auto& fontManager = FontManager::getInstance();
    fontManager.renderText(renderer, text, bounds.x, bounds.y + bounds.height, Color(0, 0, 0));
}

bool TextView::handleEvent(const Event& event) {
    return false;
}

void TextView::setText(const std::string& text) {
    this->text = text;
} 