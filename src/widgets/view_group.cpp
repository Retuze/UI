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
    // Debug::Log("ViewGroup::dispatchEvent - ", typeid(*this).name());
    
    // 1. 如果控件不可见，不处理任何事件
    if (!visible) {
        return false;
    }

    // 2. 判断是否要拦截事件
    bool intercepted = onInterceptEvent(event);
    // Debug::Log("  ViewGroup intercepted: ", intercepted);

    // 3. 如果没有拦截，让子控件处理（从上到下）
    if (!intercepted) {
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            if ((*it)->isVisible() && (*it)->dispatchEvent(event)) {
                // Debug::Log("  ViewGroup child handled event");
                return true;
            }
        }
    }
    
    // 4. 如果拦截了或子控件没处理，自己处理（从下到上冒泡）
    bool handled = onEvent(event);
    // Debug::Log("  ViewGroup self handled: ", handled);
    return handled;
}


void ViewGroup::onLayout() {
    if (layout) {
        layout->setBounds(bounds);
        layout->arrange(children);
    }
    needsLayout = false;
} 