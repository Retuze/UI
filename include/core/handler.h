#pragma once
#include "core/looper.h"
#include "core/message.h"

class Handler {
public:
    explicit Handler(Looper* looper) {
        if (!looper) {
            throw std::invalid_argument("Null looper provided to Handler constructor");
        }
        this->looper = looper;
    }
    virtual ~Handler();
    
    // 发送普通消息
    void sendMessage(Message* msg);
    
    // 发送延迟消息
    void sendMessageDelayed(Message* msg, int64_t delayMillis);
    
    // 发送定时消息
    void sendMessageAtTime(Message* msg, int64_t uptimeMillis);
    
    // 发送空消息
    void sendEmptyMessage(int what);
    
    // 发送延迟空消息
    void sendEmptyMessageDelayed(int what, int64_t delayMillis);
    
    // 发送Runnable
    void post(std::function<void()> r);
    
    // 发送延迟Runnable
    void postDelayed(std::function<void()> r, int64_t delayMillis);
    
    virtual void handleMessage(Message& message);
    
    friend class UIThread;
    
protected:
    Looper* looper;
}; 