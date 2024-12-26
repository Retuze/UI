#include "widgets/button.h"

Button::Button(const std::string& text) 
    : text(text), pressed(false) {}

void Button::draw(Renderer* renderer) {
    Color bgColor = pressed ? Color(200, 200, 200) : Color(220, 220, 220);
    renderer->fillRect(bounds, bgColor);
}

bool Button::handleEvent(const Event& event) {
    if (!bounds.contains(event.position)) {
        return false;
    }

    switch (event.type) {
        case EventType::MousePress:
            pressed = true;
            return true;
        case EventType::MouseRelease:
            if (pressed && onClick) {
                onClick();
            }
            pressed = false;
            return true;
        default:
            return false;
    }
}

void Button::setText(const std::string& text) {
    this->text = text;
}

void Button::setOnClickListener(std::function<void()> listener) {
    onClick = std::move(listener);
} 