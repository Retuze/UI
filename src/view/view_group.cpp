#include "view/view_group.h"

ViewGroup::ViewGroup() = default;

ViewGroup::~ViewGroup() {
    removeAllViews();
}

void ViewGroup::addView(View* child) {
    if (!child) return;
    
    if (child->parent) {
        child->parent->removeView(child);
    }
    
    children.push_back(child);
    child->parent = this;
    requestLayout();
}

void ViewGroup::removeView(View* child) {
    if (!child) return;
    
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->parent = nullptr;
        children.erase(it);
        requestLayout();
    }
}

void ViewGroup::removeAllViews() {
    for (auto* child : children) {
        child->parent = nullptr;
        delete child;
    }
    children.clear();
    requestLayout();
}

void ViewGroup::setLayout(Layout* newLayout) {
    layoutManager.reset(newLayout);
    requestLayout();
}

void ViewGroup::measure(int widthMeasureSpec, int heightMeasureSpec) {
    // 先测量所有子视图
    for (auto* child : children) {
        if (child->isVisible()) {
            measureChild(child, widthMeasureSpec, heightMeasureSpec);
        }
    }
    
    // 然后测量自己
    onMeasure(widthMeasureSpec, heightMeasureSpec);
}

void ViewGroup::layout(int left, int top, int right, int bottom) {
    View::layout(left, top, right, bottom);
    
    bool changed = needsLayout;
    onLayout(changed, left, top, right, bottom);
    
    // 布局所有子视图
    for (auto* child : children) {
        if (child->isVisible() && !child->bounds.isEmpty()) {
            child->layout(
                child->bounds.x,
                child->bounds.y,
                child->bounds.x + child->bounds.width,
                child->bounds.y + child->bounds.height
            );
        }
    }
}

void ViewGroup::onDraw(RenderContext& context) {
    // ViewGroup默认透明，只绘制子视图
    for (auto* child : children) {
        if (child->isVisible()) {
            child->draw(context);
        }
    }
}

bool ViewGroup::dispatchEvent(const Event& event) {
    if (!visible) return false;
    
    // 从上到下分发事件给子视图
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if ((*it)->isVisible() && (*it)->dispatchEvent(event)) {
            return true;
        }
    }
    
    // 如果子视图都没处理，自己处理
    return onEvent(event);
}

void ViewGroup::measureChild(View* child, int parentWidthMeasureSpec, int parentHeightMeasureSpec) {
    if (layoutManager) {
        layoutManager->measure(child, parentWidthMeasureSpec, parentHeightMeasureSpec);
    } else {
        child->measure(parentWidthMeasureSpec, parentHeightMeasureSpec);
    }
} 