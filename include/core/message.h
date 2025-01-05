#pragma once
#include <functional>
#include <memory>
#include <chrono>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>

class Handler;

// 消息定义
class Message {
public:
    int what = 0;
    int arg1 = 0;
    int arg2 = 0;
    void* data = nullptr;
    std::function<void()> callback;
    int64_t when = 0;
    Handler* target = nullptr;
    
    void reset() {
        what = 0;
        arg1 = 0;
        arg2 = 0;
        data = nullptr;
        callback = nullptr;
        when = 0;
        target = nullptr;
    }

    static std::unique_ptr<Message> obtain();
    static void recycle(std::unique_ptr<Message> msg);
    static void clearPool();
    // 在 Message 类中添加
};

// 消息队列定义
class MessageQueue {
public:
    using IdleHandler = std::function<bool()>;
    
    void post(std::function<void()> message);
    void postDelayed(std::function<void()> message, int64_t delayMillis);
    void processNextMessage();
    
    void enqueueMessage(std::unique_ptr<Message> msg, Handler* handler);
    void addIdleHandler(IdleHandler handler);
    void removeIdleHandler(const IdleHandler& handler);
    int postSyncBarrier();
    void removeSyncBarrier(int token);
    void removeMessagesForHandler(Handler* handler);
    void quit();
    void removeAllMessages();

private:
    struct MessageComparer {
        bool operator()(const std::unique_ptr<Message>& a, 
                       const std::unique_ptr<Message>& b) {
            return a->when > b->when;
        }
    };
    
    std::priority_queue<
        std::unique_ptr<Message>,
        std::vector<std::unique_ptr<Message>>,
        MessageComparer
    > messageQueue;
    
    std::vector<IdleHandler> idleHandlers;
    std::mutex mutex;
    std::condition_variable messageAvailable;
    bool quitting = false;

    static int64_t getCurrentTimeNanos();
    static constexpr int64_t millisToNanos(int64_t millis) {
        return millis * 1000000LL;
    }
    
    static constexpr int64_t nanosToMillis(int64_t nanos) {
        return nanos / 1000000;
    }
};

// 消息池定义
class MessagePool {
public:
    static std::unique_ptr<Message> obtain();
    static void recycle(std::unique_ptr<Message> msg);
    
private:
    static constexpr size_t MAX_POOL_SIZE = 50;
    static std::vector<std::unique_ptr<Message>> pool;
    static std::mutex mutex;
};