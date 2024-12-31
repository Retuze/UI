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
    
private:
    UIThread() = default;
    ~UIThread() = default;
    
    void threadLoop();
    
    std::unique_ptr<Handler> uiHandler;
    std::thread thread;
    std::thread::id uiThreadId;  // 新增：保存UI线程ID
    bool running = false;
}; 