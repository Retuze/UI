#include "widgets/widget.h"
// #include "core/debug.h"

Widget* Widget::capturedWidget = nullptr;

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
    // Debug::Log("Widget::dispatchEvent - ", typeid(*this).name());
    
    // 1. 如果控件不可见，不处理任何事件
    if (!visible) {
        return false;
    }

    // 2. 先让子控件处理事件（从上到下）
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if ((*it)->isVisible() && (*it)->dispatchEvent(event)) {
            // Debug::Log("  Child handled event");
            return true;
        }
    }
    
    // 3. 子控件没处理，自己处理（从下到上冒泡）
    bool handled = onEvent(event);
    // Debug::Log("  Self handled: ", handled);
    return handled;
}

bool Widget::onEvent(const Event& event) {
    // 如果是捕获的控件，不检查坐标
    if (this == capturedWidget) {
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

    // 对于非捕获的控件，检查坐标
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

void Widget::setFocus(bool focus) {
    this->focused = focus;
} 