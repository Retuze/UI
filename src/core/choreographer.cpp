#include "core/choreographer.h"

Choreographer& Choreographer::getInstance() {
    static Choreographer instance;
    return instance;
}

void Choreographer::postFrameCallback(std::function<void(std::chrono::nanoseconds)> callback, const void* token) {
    std::lock_guard<std::mutex> lock(mutex);
    frameCallbacks.push_back(FrameCallback{std::move(callback), token});
}

void Choreographer::removeFrameCallback(const void* token) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::remove_if(frameCallbacks.begin(), frameCallbacks.end(),
        [token](const auto& cb) {
            return cb.token == token;
        });
    frameCallbacks.erase(it, frameCallbacks.end());
}

void Choreographer::doFrame(std::chrono::nanoseconds frameTimeNanos) {
    std::vector<FrameCallback> callbacks;
    {
        std::lock_guard<std::mutex> lock(mutex);
        callbacks = frameCallbacks;
    }
    
    for (const auto& cb : callbacks) {
        cb.callback(frameTimeNanos);
    }
} 