#pragma once
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>

class Choreographer {
public:
    enum CallbackType {
        INPUT = 0,
        ANIMATION = 1,
        TRAVERSAL = 2,
        COMMIT = 3
    };
    
    static Choreographer& getInstance();
    
    void postCallback(CallbackType type, 
                     std::function<void(std::chrono::nanoseconds)> callback,
                     const void* token = nullptr);
                     
    void removeCallback(CallbackType type, const void* token);
    
private:
    struct CallbackRecord {
        CallbackType type;
        std::function<void(std::chrono::nanoseconds)> callback;
        const void* token;
    };
    
    std::vector<CallbackRecord> callbacks[4]; // 4种类型的回调队列
    std::mutex mutex;
}; 