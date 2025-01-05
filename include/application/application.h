#pragma once
#include "core/context.h"
#include "core/event.h"
#include "view/window_manager.h"
#include <vector>
#include <memory>

class Activity;
class WindowManagerImpl;
class RenderContext;

class Application : public Context {
public:
    static Application& getInstance();
    
    // Context接口实现
    WindowManager* getWindowManager() override { return windowManager.get(); }
    RenderContext* getRenderContext() override { return renderContext.get(); }
    Application* getApplication() override { return this; }
    std::string getResourcePath() const override;
    bool checkPermission(const std::string& permission) override;
    
    // 生命周期管理
    void onCreate();
    void onTerminate();
    
    // Activity管理
    void startActivity(Activity* activity);
    void finishActivity(Activity* activity);
    
    // 事件处理
    bool pollEvent(Event& event);
    void dispatchEvent(const Event& event);
    void render();
    
    Activity* getCurrentActivity() const { return currentActivity; }
    
private:
    std::vector<Activity*> activities;
    Activity* currentActivity = nullptr;
    std::unique_ptr<WindowManager> windowManager;
    std::unique_ptr<RenderContext> renderContext;
    
    // 初始化相关
    void initializeRenderSystem();
    void initializeResourceSystem();
    void onSystemReady();
    
    // 清理相关
    void cleanupActivities();
    void cleanupRenderSystem();
    void cleanupResources();
};
