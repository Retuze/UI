#include "view/view.h"
#include "view/view_group.h"

View::View() = default;

void View::measure(int widthMeasureSpec, int heightMeasureSpec) {
    // 默认实现：使用建议的尺寸
    int width = MeasureSpec::getSize(widthMeasureSpec);
    int height = MeasureSpec::getSize(heightMeasureSpec);
    setMeasuredDimension(width, height);
}

void View::layout(int left, int top, int right, int bottom) {
    bool changed = bounds.x != left || bounds.y != top ||
                  bounds.width != (right - left) ||
                  bounds.height != (bottom - top);
                  
    bounds.x = left;
    bounds.y = top;
    bounds.width = right - left;
    bounds.height = bottom - top;
    
    needsLayout = false;
}

void View::draw(RenderContext& context) {
    if (!visible) return;
    
    context.save();
    context.clipRect(bounds);
    onDraw(context);
    context.restore();
}

bool View::dispatchEvent(const Event& event) {
    if (!visible) return false;
    return onEvent(event);
}

bool View::onEvent(const Event& event) {
    // 基础事件处理
    return false;
}

void View::setPosition(int x, int y) {
    if (bounds.x != x || bounds.y != y) {
        bounds.x = x;
        bounds.y = y;
        invalidate();
    }
}

void View::setSize(int width, int height) {
    if (bounds.width != width || bounds.height != height) {
        bounds.width = width;
        bounds.height = height;
        requestLayout();
    }
}

void View::setBounds(const Rect& bounds) {
    setPosition(bounds.x, bounds.y);
    setSize(bounds.width, bounds.height);
}

void View::setVisible(bool visible) {
    if (this->visible != visible) {
        this->visible = visible;
        invalidate();
    }
}

void View::invalidate() {
    // 向上传递重绘请求
    if (parent) {
        parent->invalidate();
    }
}

void View::requestLayout() {
    needsLayout = true;
    if (parent) {
        parent->requestLayout();
    }
}

void View::setMeasuredDimension(int width, int height) {
    measuredWidth = width;
    measuredHeight = height;
} 