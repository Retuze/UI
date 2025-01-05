#include "view/view_root.h"
#include "core/logger.h"

LOG_TAG("ViewRoot");

ViewRoot::ViewRoot() : layoutRequested(false), needsRedraw(true) {
    // 初始化时需要一次渲染
}

void ViewRoot::setView(View* view) {
    if (hostView != view) {
        hostView = view;
        requestLayout();
    }
}

void ViewRoot::requestLayout() {
    // 如果渲染已暂停，不要请求布局
    if (!layoutRequested && !UIThread::getInstance().isRenderingPaused()) {
        layoutRequested = true;
        LOGI("Layout requested");
    }
}

void ViewRoot::performTraversals() {
    if (!hostView || !renderContext) {
        return;
    }
    
    // 只在需要时才执行测量和布局
    if (layoutRequested) {
        layoutRequested = false;
        performMeasure();
        performLayout();
        needsRedraw = true;  // 布局改变需要重绘
    }
    
    // 只在需要时才执行绘制
    if (needsRedraw) {
        needsRedraw = false;
        performDraw();
    }
}

void ViewRoot::performMeasure() {
    LOGI("Performing measure");
    
    // 获取窗口尺寸作为根视图的测量规格
    int width = renderContext->getWidth();
    int height = renderContext->getHeight();
    
    int widthSpec = MeasureSpec::makeMeasureSpec(width, MeasureSpec::EXACTLY);
    int heightSpec = MeasureSpec::makeMeasureSpec(height, MeasureSpec::EXACTLY);
    
    // 测量根视图
    hostView->measure(widthSpec, heightSpec);
}

void ViewRoot::performLayout() {
    LOGI("Performing layout");
    
    // 使用测量后的尺寸进行布局
    int width = hostView->getMeasuredWidth();
    int height = hostView->getMeasuredHeight();
    
    hostView->layout(0, 0, width, height);
}

void ViewRoot::performDraw() {
    if (!renderContext) {
        return;
    }
    
    LOGI("Performing draw");
    
    // 清空背景为白色
    renderContext->clear();
    
    // 绘制视图树
    hostView->draw(*renderContext);
    
    // 提交绘制结果
    renderContext->present();
}

void ViewRoot::invalidate() {
    needsRedraw = true;
}
