#include "widgets/view_group.h"
// #include "core/debug.h"

ViewGroup::ViewGroup() : layout(nullptr), needsLayout(true) {}

ViewGroup::~ViewGroup() {
    // 清理子控件
    for (auto* child : children) {
        delete child;
    }
    delete layout;
}

void ViewGroup::addView(Widget* child) {
    if (child && child->getParent() != this) {
        if (child->getParent()) {
            child->getParent()->removeChild(child);
        }
        children.push_back(child);
        child->setParent(this);
        requestLayout();
    }
}

void ViewGroup::removeView(Widget* child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->setParent(nullptr);
        children.erase(it);
        requestLayout();
    }
}

void ViewGroup::removeAllViews() {
    for (auto* child : children) {
        child->setParent(nullptr);
        delete child;
    }
    children.clear();
    requestLayout();
}

void ViewGroup::setLayout(Layout* newLayout) {
    delete layout;
    layout = newLayout;
    if (layout) {
        layout->setBounds(bounds);
    }
    requestLayout();
}

void ViewGroup::requestLayout() {
    needsLayout = true;
}

void ViewGroup::draw(Renderer* renderer) {
    if (needsLayout) {
        onLayout();
    }
    
    // 绘制子控件
    for (auto* child : children) {
        if (child->isVisible()) {
            child->draw(renderer);
        }
    }
}

bool ViewGroup::dispatchEvent(const Event& event) {
    if (!visible) {
        return false;
    }

    // 1. 如果有控件捕获了事件，直接发送给它
    if (Widget::getCapturedWidget()) {
        return Widget::getCapturedWidget()->onEvent(event);
    }

    // 2. 正常的事件分发流程
    bool intercepted = onInterceptEvent(event);
    
    if (!intercepted) {
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            if ((*it)->isVisible() && (*it)->dispatchEvent(event)) {
                return true;
            }
        }
    }
    
    return onEvent(event);
}


void ViewGroup::onLayout() {
    if (layout) {
        layout->setBounds(bounds);
        layout->arrange(children);
    }
    needsLayout = false;
} 