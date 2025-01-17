#include "application/application.h"
#include "graphics/ui_thread.h"
#include "core/logger.h"
#include "core/choreographer.h"
#include <algorithm>
#include <thread>
#include <chrono>
#include "view/view_root.h"
#include "activity/activity.h"
#include "view/window_manager.h"

LOG_TAG("Application");

Application& Application::getInstance() {
    static Application instance;
    return instance;
}

void Application::onCreate() {
    // 1. 核心系统
    Logger::instance().initialize();
    UIThread::getInstance().initialize();
    
    // 2. 渲染系统
    initializeRenderSystem();
    
    // 3. 资源系统
    initializeResourceSystem();
    
    // 4. 通知系统就绪
    onSystemReady();
}

void Application::onTerminate() {
    LOGI("Application terminating...");
    
    // 1. 禁用窗口管理器，停止接收新的渲染请求
    if (!windowManager) {
        LOGE("WindowManager is null in onTerminate");
    } else {
        LOGI("Disabling window manager");
        windowManager->setEnabled(false);
    }
    
    // 2. 暂停UI线程渲染
    LOGI("Pausing UI thread rendering");
    UIThread::getInstance().pauseRendering();
    
    // 3. 等待最后一帧渲染完成
    LOGI("Waiting for last frame to complete");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 4. 清理 Activities
    LOGI("Cleaning up activities");
    cleanupActivities();
    
    // 5. 停止 UI 线程
    LOGI("Stopping UI thread");
    UIThread::getInstance().quit();
    
    LOGI("Cleaning up render system");
    cleanupRenderSystem();
    
    // 8. 清理其他资源
    LOGI("Cleaning up remaining resources");
    cleanupResources();
}

void Application::startActivity(const Intent& intent) {
    Activity* newActivity = createActivityInstance(intent.activityName);
    if (!newActivity) return;
    
    // 设置传递的数据
    newActivity->extras = intent.extras;
    
    // 处理Activity切换
    Activity* currentActivity = getCurrentActivity();
    handleActivityTransition(currentActivity, newActivity);
    
    // 将新Activity压入栈中
    activityStack.push(newActivity);
    
    // 初始化新Activity
    newActivity->dispatchCreate();
    newActivity->dispatchStart();
    newActivity->dispatchResume();
}

void Application::startActivityForResult(Activity* sourceActivity, const Intent& intent, int requestCode) {
    Activity* newActivity = createActivityInstance(intent.activityName);
    if (!newActivity) return;
    
    newActivity->extras = intent.extras;
    handleActivityTransition(sourceActivity, newActivity);
    activityStack.push(newActivity);
    
    // 记录请求信息，以便返回时处理
    pendingResults[newActivity] = {sourceActivity, requestCode};
    
    newActivity->dispatchCreate();
    newActivity->dispatchStart();
    newActivity->dispatchResume();
}

void Application::finishActivity(Activity* activity) {
    if (!activity || activityStack.empty()) return;
    
    activity->dispatchPause();
    activity->dispatchStop();
    activity->dispatchDestroy();
    
    activityStack.pop();
    
    // 恢复上一个Activity
    if (!activityStack.empty()) {
        Activity* previousActivity = activityStack.top();
        previousActivity->dispatchStart();
        previousActivity->dispatchResume();
    }
    
    delete activity;
}

void Application::finishActivityWithResult(Activity* activity, int resultCode, const std::map<std::string, std::any>& resultData) {
    if (!activity) return;
    
    // 查找是否有待处理的结果
    auto it = pendingResults.find(activity);
    if (it != pendingResults.end()) {
        Activity* sourceActivity = it->second.sourceActivity;
        int requestCode = it->second.requestCode;
        
        // 传递结果给源Activity
        deliverActivityResult(sourceActivity, requestCode, resultCode, resultData);
        pendingResults.erase(it);
    }
    
    finishActivity(activity);
}

void Application::moveToBackground() {
    if (appState != AppState::Running) return;
    
    appState = AppState::Background;
    lastBackgroundTime = std::chrono::steady_clock::now();
    
    // 暂停当前Activity
    if (Activity* currentActivity = getCurrentActivity()) {
        currentActivity->dispatchPause();
        currentActivity->dispatchStop();
        currentActivity->dispatchSaveInstanceState();
    }
    
    pauseNonEssentialTasks();
    releaseNonEssentialResources();
}

void Application::moveToForeground() {
    if (appState != AppState::Background) return;
    
    appState = AppState::Running;
    restoreResources();
    resumeNonEssentialTasks();
    
    // 恢复当前Activity
    if (Activity* currentActivity = getCurrentActivity()) {
        currentActivity->dispatchRestoreInstanceState();
        currentActivity->dispatchStart();
        currentActivity->dispatchResume();
    }
}

Activity* Application::createActivityInstance(const std::string& activityName) {
    // 这里需要一个Activity工厂来创建具体的Activity实例
    // 可以通过注册表或反射机制实现
    return nullptr; // 具体实现需要根据你的Activity注册机制来完成
}

void Application::handleActivityTransition(Activity* from, Activity* to) {
    if (from) {
        from->dispatchPause();
        from->dispatchStop();
        from->dispatchSaveInstanceState();
    }
}

void Application::deliverActivityResult(Activity* target, int requestCode, int resultCode, const std::map<std::string, std::any>& data) {
    if (target) {
        target->onActivityResult(requestCode, resultCode, data);
    }
}

bool Application::pollEvent(Event& event) {
    // TODO: 使用新的事件系统处理事件
    return true;
}

void Application::dispatchEvent(const Event& event) {
    // 如果当前Activity存在，将事件分发给它的内容视图
    if (Activity* currentActivity = getCurrentActivity()) {
        if (View* contentView = currentActivity->getContentView()) {
            contentView->dispatchEvent(event);
        }
    }
}

void Application::render() {
    if (!windowManager) {
        LOGE("WindowManager is null in render");
        return;
    }
    windowManager->performTraversals();
}

std::string Application::getResourcePath() const {
    // 返回资源路径，这里简单返回当前目录
    return "./resources";
}

bool Application::checkPermission(const std::string& permission) {
    // 简单的权限检查实现
    return true;
}

void Application::initializeRenderSystem() {
    // 1. 创建主窗口 Surface
    SurfaceConfig config{
        .width = 800,
        .height = 600,
        .format = PixelFormat::BGRA8888_LE(),
        .bufferCount = 2,
        .vsyncEnabled = true
    };
    mainSurface = Surface::create(config);
    if (!mainSurface->initialize()) {
        throw std::runtime_error("Failed to initialize surface");
    }
    
    // 2. 创建渲染上下文
    renderContext = std::make_unique<RenderContext>();
    
    // 3. 初始化窗口管理器
    windowManager = std::make_unique<WindowManager>();
    windowManager->setRenderContext(renderContext.get());
}

void Application::initializeResourceSystem() {

}

void Application::onSystemReady() {
    Logger::instance().info("Application", "System initialization completed");
}

void Application::cleanupActivities() {
    // 先暂停当前Activity
    if (Activity* currentActivity = getCurrentActivity()) {
        currentActivity->dispatchPause();
        currentActivity->dispatchStop();
    }
    
    // 销毁所有Activity
    while (!activityStack.empty()) {
        Activity* activity = activityStack.top();
        activityStack.pop();
        activity->dispatchDestroy();
        delete activity;
    }
}

void Application::cleanupRenderSystem() {
    if (mainSurface) {
        mainSurface->destroy();
        mainSurface = nullptr;
    }
    windowManager = nullptr;
    renderContext = nullptr;
}

void Application::cleanupResources() {
    Message::clearPool();
    Logger::instance().info("Application", "Application terminated");
    Logger::instance().shutdown();
}