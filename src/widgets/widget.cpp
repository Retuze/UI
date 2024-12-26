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

bool Widget::dispatchEvent(const Event& event) {
    // 先给子控件处理事件的机会
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if ((*it)->dispatchEvent(event)) {
            return true;
        }
    }
    
    // 如果子控件没有处理，自己处理
    return onEvent(event);
}

bool Widget::onEvent(const Event& event) {
    switch (event.type) {
        case EventType::MousePress:
            return onMousePress(event);
        case EventType::MouseRelease:
            return onMouseRelease(event);
        case EventType::MouseMove:
            return onMouseMove(event);
        default:
            return false;
    }
}

bool Widget::onMousePress(const Event& event) {
    return false;
}

bool Widget::onMouseRelease(const Event& event) {
    return false;
}

bool Widget::onMouseMove(const Event& event) {
    return false;
}

void Widget::setFocus(bool focus) {
    this->focused = focus;
} 