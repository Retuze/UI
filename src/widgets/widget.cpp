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
    // 如果控件不可见,不处理任何事件
    if (!visible) {
        return false;
    }

    // 1. 先询问自己是否要拦截事件
    if (onInterceptEvent(event)) {
        // 如果拦截,则自己处理
        return onEvent(event);
    }

    // 2. 给子控件分发事件(从上到下)
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if ((*it)->dispatchEvent(event)) {
            return true;
        }
    }
    
    // 3. 如果子控件都没处理,自己处理
    return onEvent(event);
}

bool Widget::onEvent(const Event& event) {
    // 对于鼠标事件,检查是否在控件范围内
    if (event.type == EventType::MousePress || 
        event.type == EventType::MouseRelease || 
        event.type == EventType::MouseMove) {
        if (!bounds.contains(event.position)) {
            return false;
        }
    }

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