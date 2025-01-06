#pragma once
#include <functional>
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

    static Message* obtain();
    static void recycle(Message* msg);
    static void clearPool();
    // 在 Message 类中添加

    ~Message() = default;
};

// 消息队列定义
class MessageQueue {
public:
    using IdleHandler = std::function<bool()>;
    
    void post(std::function<void()> message);
    void postDelayed(std::function<void()> message, int64_t delayMillis);
    void processNextMessage();
    
    void enqueueMessage(Message* msg, Handler* handler);
    void addIdleHandler(IdleHandler handler);
    void removeIdleHandler(const IdleHandler& handler);
    int postSyncBarrier();
    void removeSyncBarrier(int token);
    void removeMessagesForHandler(Handler* handler);
    void quit();
    void removeAllMessages();

private:
    struct MessageComparer {
        bool operator()(Message* a, Message* b) {
            return a->when > b->when;
        }
    };
    
    std::priority_queue<
        Message*,
        std::vector<Message*>,
        MessageComparer
    > messageQueue;
    
    std::vector<IdleHandler> idleHandlers;
    std::mutex mutex;
    std::condition_variable messageAvailable;
    bool quitting = false;

    static int64_t getCurrentTimeNanos();
};

// 消息池定义
class MessagePool {
public:
    static Message* obtain();
    static void recycle(Message* msg);
    
private:
    static constexpr size_t MAX_POOL_SIZE = 50;
    static std::vector<Message*> pool;
    static std::mutex mutex;
};