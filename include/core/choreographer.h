#pragma once
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

class Surface;

class Choreographer {
public:
    using FrameCallback = std::function<void(int64_t frameTimeNanos)>;
    
    static Choreographer& getInstance();
    
    // 添加帧回调到队列
    void postFrameCallback(FrameCallback callback);
    
    // 设置 Surface 用于 VSync
    void setSurface(Surface* surface);
    
    // 控制
    bool start();
    void stop();
    
    // 设置后备帧率（VSync 不可用时使用）
    void setFallbackFrameRate(int fps);

private:
    Choreographer();
    ~Choreographer() = default;
    
    Surface* surface{nullptr};
    std::atomic<bool> running{false};
    std::unique_ptr<std::thread> vsyncThread;
    
    std::mutex callbackMutex;
    std::queue<FrameCallback> frameCallbacks;
    
    int64_t fallbackFrameIntervalNanos{16666667}; // 默认 60fps
    
    void vsyncLoop();
    void executeCallbacks(int64_t frameTimeNanos);
}; 