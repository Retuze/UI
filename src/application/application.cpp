#include "application/application.h"
#include <algorithm>

Application& Application::getInstance() {
    static Application instance;
    return instance;
}

void Application::onCreate() {
    // 初始化渲染上下文
    renderContext = std::make_unique<RenderContext>();
    renderContext->initialize(800, 600);
}

void Application::onTerminate() {
    // 清理所有Activity
    for (auto* activity : activities) {
        if (activity->isStarted()) {
            activity->dispatchStop();
        }
        if (activity->getState() != Activity::ActivityState::Destroyed) {
            activity->dispatchDestroy();
        }
        delete activity;
    }
    activities.clear();
    currentActivity = nullptr;
    
    // 清理渲染上下文
    renderContext.reset();
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