#include "application/application.h"
#include "graphics/ui_thread.h"
#include "core/logger.h"
#include "core/choreographer.h"
#include "graphics/font_manager.h"
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
    // 1. 禁用窗口管理器，停止接收新的渲染请求
    if (windowManager) {
        windowManager->setEnabled(false);
    }
    
    // 2. 暂停UI线程渲染
    UIThread::getInstance().pauseRendering();
    
    // 3. 等待最后一帧渲染完成
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 4. 清理 Activities (此时不会触发新的渲染)
    cleanupActivities();
    
    // 5. 停止 UI 线程
    UIThread::getInstance().quit();
    
    windowManager.reset();
    renderContext.reset();
    
    // 8. 清理其他资源
    cleanupResources();
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
    if (windowManager) {
        windowManager->performTraversals();
    }
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
    // 初始化渲染上下文
    renderContext = std::make_unique<RenderContext>();
    if (!renderContext->initialize(800, 600)) {
        throw std::runtime_error("Failed to initialize render context");
    }
    
    // 初始化窗口管理器
    windowManager = std::make_unique<WindowManager>();
    windowManager->setRenderContext(renderContext.get());
}

void Application::initializeResourceSystem() {
    // 初始化字体管理器
    auto& fontManager = FontManager::getInstance();
    if (!fontManager.initialize()) {
        throw std::runtime_error("Failed to initialize FontManager");
    }
    
    // 加载默认字体
    if (!fontManager.loadFont("C:/Windows/Fonts/msyh.ttc", 32)) {
        throw std::runtime_error("Failed to load default font");
    }
}

void Application::onSystemReady() {
    Logger::instance().info("Application", "System initialization completed");
}

void Application::cleanupActivities() {
    // 先暂停当前Activity
    if (currentActivity) {
        currentActivity->dispatchPause();
        currentActivity->dispatchStop();
    }
    
    // 销毁所有Activity
    for (auto* activity : activities) {
        activity->dispatchDestroy();
        delete activity;
    }
    activities.clear();
    currentActivity = nullptr;
}

void Application::cleanupRenderSystem() {
    if (renderContext && renderContext->getSurface()) {
        renderContext->getSurface()->close();
    }
    windowManager.reset();
    renderContext.reset();
}

void Application::cleanupResources() {
    Message::clearPool();
    Logger::instance().info("Application", "Application terminated");
    Logger::instance().shutdown();
}