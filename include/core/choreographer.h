#pragma once
#include <functional>
#include <chrono>
#include <atomic>
#include <thread>
#include <windows.h>
#include <dwmapi.h>

class Choreographer {
public:
    static Choreographer& getInstance();
    
    // 设置帧回调
    void setFrameCallback(std::function<void(int64_t frameTimeNanos)> callback);
    
    // 启动和停止
    void start();
    void stop();
    
    // 设置帧率
    void setFrameRate(int fps) {
        frameIntervalNanos = static_cast<int64_t>(1000000000.0 / fps);
    }
    
private:
    Choreographer();
    
    // VSync信号循环
    void vsyncLoop();
    
    // 检查DWM合成是否启用
    bool isDwmEnabled() const;
    
    std::function<void(int64_t)> frameCallback;
    int64_t frameIntervalNanos = 16666666; // 默认60fps
    std::atomic<bool> running{false};
    std::unique_ptr<std::thread> vsyncThread;
    bool dwmSupported = false;
}; 