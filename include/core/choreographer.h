#pragma once
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>

class Choreographer {
public:
    struct FrameCallback {
        std::function<void(std::chrono::nanoseconds)> callback;
        const void* token = nullptr;
    };
    
    static Choreographer& getInstance();
    void postFrameCallback(std::function<void(std::chrono::nanoseconds)> callback, const void* token = nullptr);
    void removeFrameCallback(const void* token);
    void doFrame(std::chrono::nanoseconds frameTimeNanos);
    
private:
    Choreographer() = default;
    std::vector<FrameCallback> frameCallbacks;
    std::mutex mutex;
}; 