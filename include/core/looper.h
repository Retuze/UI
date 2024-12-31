#pragma once
#include "core/message.h"
#include <thread>
#include <memory>

class Looper {
public:
    static std::shared_ptr<Looper> getForThread();
    static void prepare();
    static void loop();
    static void quit();
    
    MessageQueue* getQueue() { return queue.get(); }
    
private:
    Looper();
    std::unique_ptr<MessageQueue> queue;
    static thread_local std::shared_ptr<Looper> threadLooper;
}; 