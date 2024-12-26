#include "widgets/widget.h"

Widget::Widget() : parent(nullptr), visible(true) {}

void Widget::setPosition(int x, int y) {
    bounds.x = x;
    bounds.y = y;
}

void Widget::setSize(int width, int height) {
    bounds.width = width;
    bounds.height = height;
}

void Widget::setBounds(const Rect& bounds) {
    this->bounds = bounds;
}

void Widget::addChild(Widget* child) {
    if (child && child->parent != this) {
        if (child->parent) {
            child->parent->removeChild(child);
        }
        child->parent = this;
        children.push_back(child);
    }
}

void Widget::removeChild(Widget* child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->parent = nullptr;
        children.erase(it);
    }
} 