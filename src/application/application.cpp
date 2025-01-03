#include "application/application.h"
#include "graphics/ui_thread.h"
#include "core/logger.h"
#include "core/choreographer.h"
#include <algorithm>
#include <thread>
#include <chrono>
#include "view/view_root_impl.h"

Application& Application::getInstance() {
    static Application instance;
    return instance;
}

void Application::onCreate() {
    // 初始化日志系统
    Logger::instance().initialize();
    
    // 初始化渲染上下文
    renderContext = std::make_unique<RenderContext>();
    if (!renderContext->initialize(800, 600)) {
        throw std::runtime_error("Failed to initialize render context");
    }
    
    // 初始化UI线程
    UIThread::getInstance().initialize();
}

void Application::onTerminate() {
    Logger::instance().info("Application", "Application terminating...");
    
    // 1. 停止UI线程
    UIThread::getInstance().quit();
    
    // 2. 停止渲染循环
    if (renderContext && renderContext->getSurface()) {
        renderContext->getSurface()->close();
    }
    
    // 3. 清理Activities
    for (auto* activity : activities) {
        delete activity;
    }
    activities.clear();
    currentActivity = nullptr;
    
    // 4. 清理其他资源
    renderContext.reset();
    Message::clearPool();
    
    // 5. 最后关闭日志系统
    Logger::instance().info("Application", "Application terminated");
    Logger::instance().shutdown();
}

void Application::startActivity(Activity* activity) {
    if (!activity) return;
    
    // 暂停当前Activity
    if (currentActivity) {
        currentActivity->dispatchPause();
        currentActivity->dispatchStop();
    }
    
    // 添加新Activity
    activities.push_back(activity);
    currentActivity = activity;
    
    // 启动新Activity
    activity->dispatchCreate();
    activity->dispatchStart();
    activity->dispatchResume();
}

void Application::finishActivity(Activity* activity) {
    if (!activity) return;
    
    // 查找Activity
    auto it = std::find(activities.begin(), activities.end(), activity);
    if (it == activities.end()) return;
    
    // 如果是当前Activity,需要先暂停
    if (activity == currentActivity) {
        activity->dispatchPause();
        activity->dispatchStop();
        currentActivity = nullptr;
        
        // 恢复上一个Activity
        if (activities.size() > 1) {
            currentActivity = activities[activities.size() - 2];
            currentActivity->dispatchStart();
            currentActivity->dispatchResume();
        }
    }
    
    // 销毁Activity
    activity->dispatchDestroy();
    activities.erase(it);
    delete activity;
}

bool Application::pollEvent(Event& event) {
    // 检查渲染上下文是否有效
    if (!renderContext || !renderContext->getSurface()) {
        return false;
    }
    
    // 从窗口系统获取事件
    return renderContext->getSurface()->pollEvent(event);
}

void Application::dispatchEvent(const Event& event) {
    // 如果当前Activity存在，将事件分发给它的内容视图
    if (currentActivity) {
        if (View* contentView = currentActivity->getContentView()) {
            contentView->dispatchEvent(event);
        }
    }
}

void Application::render() {
    if (currentActivity) {
        currentActivity->performTraversal();
    }
    
    if (!renderContext) {
        return;
    }
    
    renderContext->clear(Color(255, 255, 255));
    renderContext->present();
}