#pragma once
#include "core/context.h"
#include "core/event.h"
#include "view/window_manager.h"
#include <vector>
#include <stack>
#include <memory>
#include <chrono>
#include <map>
#include <any>

class Activity;
class WindowManagerImpl;
class RenderContext;

class Application : public Context {
public:
    // 应用程序状态
    enum class AppState {
        NotRunning,     // 未运行
        Starting,       // 正在启动
        Running,        // 正在运行（前台）
        Background,     // 后台运行
        Suspended,      // 挂起（内存中但不运行）
        Terminating     // 正在终止
    };

    static Application& getInstance();
    
    // Context接口实现
    WindowManager* getWindowManager() override { return windowManager.get(); }
    RenderContext* getRenderContext() override { return renderContext.get(); }
    Application* getApplication() override { return this; }
    std::string getResourcePath() const override;
    bool checkPermission(const std::string& permission) override;
    
    // 应用程序生命周期管理
    void onCreate();
    void onTerminate();
    
    // 后台运行相关
    void moveToBackground();    // 切换到后台
    void moveToForeground();    // 切换到前台
    void onLowMemory();         // 系统内存不足回调
    void onTrimMemory(int level); // 清理内存回调
    bool isInForeground() const { return appState == AppState::Running; }
    
    // Activity导航相关
    struct Intent {
        std::string activityName;                    // 目标Activity名称
        std::map<std::string, std::any> extras;      // 携带的额外数据
    };

    // Activity栈管理与导航
    void startActivity(const Intent& intent);
    void startActivityForResult(Activity* sourceActivity, const Intent& intent, int requestCode);
    void finishActivity(Activity* activity);
    void finishActivityWithResult(Activity* activity, int resultCode, const std::map<std::string, std::any>& resultData);
    Activity* getCurrentActivity() const { return !activityStack.empty() ? activityStack.top() : nullptr; }
    
    // 事件处理
    bool pollEvent(Event& event);
    void dispatchEvent(const Event& event);
    void render();
    
    // 应用状态查询
    AppState getState() const { return appState; }
    bool isRunning() const { return appState == AppState::Running || appState == AppState::Background; }
    
private:
    AppState appState = AppState::NotRunning;
    std::stack<Activity*> activityStack;
    std::unique_ptr<WindowManager> windowManager;
    std::unique_ptr<Surface> mainSurface;
    std::unique_ptr<RenderContext> renderContext;
    
    // 后台运行相关
    std::chrono::steady_clock::time_point lastBackgroundTime;
    bool isBackgroundRenderingEnabled = false;
    
    // 初始化相关
    void initializeRenderSystem();
    void initializeResourceSystem();
    void onSystemReady();
    
    // 清理相关
    void cleanupActivities();
    void cleanupRenderSystem();
    void cleanupResources();
    
    // 后台任务管理
    void pauseNonEssentialTasks();
    void resumeNonEssentialTasks();
    void handleBackgroundWork();
    
    // 资源管理
    void releaseNonEssentialResources();
    void restoreResources();

    // Activity导航相关的私有方法
    Activity* createActivityInstance(const std::string& activityName);
    void handleActivityTransition(Activity* from, Activity* to);
    void deliverActivityResult(Activity* target, int requestCode, int resultCode, const std::map<std::string, std::any>& data);

    // 用于存储Activity返回结果的信息
    struct PendingResult {
        Activity* sourceActivity;
        int requestCode;
    };
    std::map<Activity*, PendingResult> pendingResults;
};
