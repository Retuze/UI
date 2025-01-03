#include "view/view_root_impl.h"
#include "view/measure_spec.h"

void ViewRootImpl::setView(View* view) {
    hostView = view;
    requestLayout();
}

void ViewRootImpl::requestLayout() {
    layoutRequested = true;
}

void ViewRootImpl::invalidate() {
    invalidated = true;
}

void ViewRootImpl::performTraversal() {
    if (layoutRequested) {
        // 执行测量和布局
        performMeasure(800, 600);  // 使用窗口尺寸
        performLayout();
        layoutRequested = false;
    }
    
    if (invalidated) {
        // 执行绘制
        // performDraw() 需要 RenderContext 参数
        invalidated = false;
    }
}

void ViewRootImpl::performMeasure(int width, int height) {
    if (hostView) {
        int widthSpec = MeasureSpec::makeMeasureSpec(width, MeasureSpec::EXACTLY);
        int heightSpec = MeasureSpec::makeMeasureSpec(height, MeasureSpec::EXACTLY);
        hostView->measure(widthSpec, heightSpec);
    }
}

void ViewRootImpl::performLayout() {
    if (hostView) {
        hostView->layout(0, 0, hostView->getMeasuredWidth(), hostView->getMeasuredHeight());
    }
}

void ViewRootImpl::performDraw(RenderContext& context) {
    if (hostView) {
        hostView->draw(context);
    }
} 