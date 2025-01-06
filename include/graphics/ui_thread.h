#pragma once
#include <thread>
#include <memory>
#include <functional>
#include "core/handler.h"

class UIThread {
public:
    static UIThread& getInstance();
    
    void initialize();
    void quit();
    
    // 在UI线程执行任务
    void runOnUiThread(std::function<void()> task);
    void runOnUiThreadDelayed(std::function<void()> task, int64_t delayMillis);
    
    // 判断当前是否为UI线程
    bool isUiThread() const;
    
    Looper* getLooper() const { return threadLooper; }
    
    void pauseRendering();
    void resumeRendering();
    
    bool isRenderingPaused() const { return renderingPaused; }
    
private:
    UIThread();
    ~UIThread();
    
    void threadLoop();
    
    bool renderingPaused = false;
    std::atomic<bool> running{false};
    std::thread::id uiThreadId;
    std::thread thread;
    Looper* threadLooper = nullptr;
    Handler* uiHandler = nullptr;
}; 