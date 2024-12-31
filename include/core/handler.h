#pragma once
#include "core/looper.h"
#include "core/message.h"
#include <memory>

class Handler {
public:
    explicit Handler(Looper* looper);
    virtual ~Handler();
    
    // 发送普通消息
    void sendMessage(std::unique_ptr<Message> msg);
    
    // 发送延迟消息
    void sendMessageDelayed(std::unique_ptr<Message> msg, int64_t delayMillis);
    
    // 发送定时消息
    void sendMessageAtTime(std::unique_ptr<Message> msg, int64_t uptimeMillis);
    
    // 发送空消息
    void sendEmptyMessage(int what);
    
    // 发送延迟空消息
    void sendEmptyMessageDelayed(int what, int64_t delayMillis);
    
    // 发送Runnable
    void post(std::function<void()> r);
    
    // 发送延迟Runnable
    void postDelayed(std::function<void()> r, int64_t delayMillis);
    
    virtual void handleMessage(const Message& msg) {}
    
protected:
    Looper* looper;
}; 