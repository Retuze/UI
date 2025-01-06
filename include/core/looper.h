#pragma once
#include "core/message.h"
#include <thread>

class Looper {
public:
    static Looper* getForThread();
    static void prepare();
    static void loop();
    static void quit();
    
    MessageQueue* getQueue() { return queue; }
    ~Looper();
    
private:
    Looper();
    
    MessageQueue* queue;
    static thread_local Looper* threadLooper;
}; 