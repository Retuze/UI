#pragma once
#include <thread>
#include <memory>
#include <functional>
#include "core/handler.h"

/**
 * @brief 渲染循环管理类，负责管理渲染线程和任务调度
 * 使用单例模式确保全局只有一个渲染循环实例
 */
class RenderLoop {
public:
    // 获取RenderLoop单例实例
    static RenderLoop& getInstance();
    
    // 启动渲染循环
    void start();
    // 停止渲染循环
    void stop();
    
    // 在渲染线程上提交任务
    void post(std::function<void()> task);
    // 在渲染线程上延迟提交任务
    void postDelayed(std::function<void()> task, int64_t delayMillis);
    
    // 检查当前是否在渲染线程上
    bool isOnRenderThread() const;
    
    // 暂停渲染
    void pauseRendering();
    // 恢复渲染
    void resumeRendering();
    
private:
    RenderLoop();
    ~RenderLoop();
    
    void run();
    
    std::atomic<bool> isRunning{false};
    std::thread::id renderThreadId;
    std::thread renderThread;
    std::unique_ptr<Looper> looper;
    std::unique_ptr<Handler> taskHandler;
}; 