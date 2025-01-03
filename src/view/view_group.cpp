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
    
    // 设置自己的bounds尺寸为测量尺寸
    bounds.width = getMeasuredWidth();
    bounds.height = getMeasuredHeight();
}

void ViewGroup::layout(int left, int top, int right, int bottom) {
    View::layout(left, top, right, bottom);
    
    bool changed = needsLayout;
    onLayout(changed, left, top, right, bottom);
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
    auto& params = child->getLayoutParams();
    
    // 处理宽度的MeasureSpec
    int childWidthSpec;
    if (params.width == LayoutParams::MATCH_PARENT) {
        // MATCH_PARENT: 使用父容器的大小
        childWidthSpec = MeasureSpec::makeMeasureSpec(
            MeasureSpec::getSize(parentWidthMeasureSpec) - paddingLeft - paddingRight,
            MeasureSpec::EXACTLY);
    } else if (params.width == LayoutParams::WRAP_CONTENT) {
        // WRAP_CONTENT: 最大不超过父容器
        childWidthSpec = MeasureSpec::makeMeasureSpec(
            MeasureSpec::getSize(parentWidthMeasureSpec) - paddingLeft - paddingRight,
            MeasureSpec::AT_MOST);
    } else {
        // 固定大小
        childWidthSpec = MeasureSpec::makeMeasureSpec(params.width, MeasureSpec::EXACTLY);
    }
    
    // 处理高度的MeasureSpec
    int childHeightSpec;
    if (params.height == LayoutParams::MATCH_PARENT) {
        childHeightSpec = MeasureSpec::makeMeasureSpec(
            MeasureSpec::getSize(parentHeightMeasureSpec) - paddingTop - paddingBottom,
            MeasureSpec::EXACTLY);
    } else if (params.height == LayoutParams::WRAP_CONTENT) {
        childHeightSpec = MeasureSpec::makeMeasureSpec(
            MeasureSpec::getSize(parentHeightMeasureSpec) - paddingTop - paddingBottom,
            MeasureSpec::AT_MOST);
    } else {
        childHeightSpec = MeasureSpec::makeMeasureSpec(params.height, MeasureSpec::EXACTLY);
    }
    
    child->measure(childWidthSpec, childHeightSpec);
}

void ViewGroup::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    // Only calculate max dimensions from already measured children
    int maxWidth = 0;
    int maxHeight = 0;
    
    for (auto* child : children) {
        if (child && child->getVisibility() != View::GONE) {
            maxWidth = std::max(maxWidth, child->getMeasuredWidth());
            maxHeight = std::max(maxHeight, child->getMeasuredHeight());
        }
    }
    
    // Consider padding
    maxWidth += getPaddingLeft() + getPaddingRight();
    maxHeight += getPaddingTop() + getPaddingBottom();
    
    setMeasuredDimension(
        MeasureSpec::resolveSize(maxWidth, widthMeasureSpec),
        MeasureSpec::resolveSize(maxHeight, heightMeasureSpec)
    );
}

void ViewGroup::onLayout(bool changed, int left, int top, int right, int bottom) {
    for (auto* child : children) {
        if (child && child->isVisible()) {
            // 使用子视图的测量尺寸和原始位置
            int childWidth = child->getMeasuredWidth();
            int childHeight = child->getMeasuredHeight();
            
            // 保持原始位置，但使用测量后的尺寸
            child->layout(
                child->bounds.x,  // 保持原始x位置
                child->bounds.y,  // 保持原始y位置
                child->bounds.x + childWidth,  // 使用测量宽度
                child->bounds.y + childHeight  // 使用测量高度
            );
        }
    }
} 