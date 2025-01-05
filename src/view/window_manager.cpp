#include "view/window_manager.h"
#include "view/decor_view.h"

void WindowManager::addView(View* view, const LayoutParams& params) {
    // 首次添加视图时创建根视图结构
    if (!rootView) {
        rootView = new DecorView();
        viewRoot = std::make_unique<ViewRoot>();
        viewRoot->setView(rootView);
        viewRoot->setRenderContext(renderContext);
    }
    
    // 将新视图添加到DecorView
    static_cast<DecorView*>(rootView)->setContentView(view);
    view->setLayoutParams(params);
    
    // 请求布局
    viewRoot->requestLayout();
}

void WindowManager::updateViewLayout(View* view, const LayoutParams& params) {
    if (view) {
        view->setLayoutParams(params);
        viewRoot->requestLayout();
    }
}

void WindowManager::removeView(View* view) {
    if (!rootView || !view) {
        return;
    }
    static_cast<DecorView*>(rootView)->setContentView(nullptr);
    
    // 只在启用状态下请求布局
    if (isEnabled) {
        viewRoot->requestLayout();
    }
}

void WindowManager::performTraversals() {
    if (!isEnabled || !viewRoot) {
        return;
    }
    viewRoot->performTraversals();
} 