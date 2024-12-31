#include "core/choreographer.h"

Choreographer& Choreographer::getInstance() {
    static Choreographer instance;
    return instance;
}

void Choreographer::postCallback(CallbackType type, 
                               std::function<void(std::chrono::nanoseconds)> callback,
                               const void* token) {
    std::lock_guard<std::mutex> lock(mutex);
    callbacks[type].push_back(CallbackRecord{type, std::move(callback), token});
}

void Choreographer::removeCallback(CallbackType type, const void* token) {
    std::lock_guard<std::mutex> lock(mutex);
    auto& queue = callbacks[type];
    auto it = std::remove_if(queue.begin(), queue.end(),
        [token](const auto& cb) {
            return cb.token == token;
        });
    queue.erase(it, queue.end());
}

void Choreographer::doFrame(std::chrono::nanoseconds frameTimeNanos) {
    // 按优先级顺序执行回调
    for (int type = 0; type < 4; type++) {
        std::vector<CallbackRecord> records;
        {
            std::lock_guard<std::mutex> lock(mutex);
            records = callbacks[type];
        }
        
        for (const auto& record : records) {
            record.callback(frameTimeNanos);
        }
    }
} 