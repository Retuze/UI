#include "view/window_manager.h"
#include "view/decor_view.h"
#include "core/logger.h"

LOG_TAG("WindowManager");

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
    if (!rootView || !view || !isEnabled) {
        return;
    }
    
    // 1. 确保视图树不再引用此视图
    if (auto* decorView = static_cast<DecorView*>(rootView)) {
        if (decorView->getContentView() == view) {
            decorView->setContentView(nullptr);
        }
    }
    
    // 2. 请求一次布局以确保视图树更新
    if (viewRoot) {
        viewRoot->requestLayout();
    }
}

void WindowManager::performTraversals() {
    if (!isEnabled || !viewRoot) {
        return;
    }
    viewRoot->performTraversals();
} 

WindowManager::~WindowManager(){
        LOGI("WindowManager destructor...");
        
        // 确保先清理ViewRoot
        LOGI("Cleaning up ViewRoot");
        viewRoot.reset();
        
        // 然后删除rootView
        LOGI("Cleaning up root view");
        delete rootView;
        rootView = nullptr;
        
        LOGI("WindowManager destroyed successfully");
    }