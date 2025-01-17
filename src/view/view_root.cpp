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
    if (!hostView) {
        LOGE("HostView is null in performTraversals");
        return;
    }
    if (!mainSurface) {
        LOGE("RenderContext is null in performTraversals");
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
    int width = mainSurface->getWidth();
    int height = mainSurface->getHeight();
    
    LOGI("ViewRoot measure with window size: %d x %d", width, height);
    
    int widthSpec = MeasureSpec::makeMeasureSpec(width, MeasureSpec::EXACTLY);
    int heightSpec = MeasureSpec::makeMeasureSpec(height, MeasureSpec::EXACTLY);
    
    hostView->measure(widthSpec, heightSpec);
    LOGI("Root view measured size: %d x %d", 
         hostView->getMeasuredWidth(), 
         hostView->getMeasuredHeight());
}

void ViewRoot::performLayout() {
    int width = hostView->getMeasuredWidth();
    int height = hostView->getMeasuredHeight();
    
    LOGI("ViewRoot layout with size: %d x %d", width, height);
    hostView->layout(0, 0, width, height);
}

void ViewRoot::performDraw() {
    if (!mainSurface) {
        return;
    }
    
    // LOGI("Performing draw");
    
    // 清空背景为白色
    mainSurface->clear();
    
    // 绘制视图树
    hostView->draw(*mainSurface);
    
    // 提交绘制结果
    mainSurface->present();
}

void ViewRoot::invalidate() {
    needsRedraw = true;
}
