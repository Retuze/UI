#pragma once
#include <functional>
#include <chrono>

class Handler;

class Message {
public:
    int what;
    void* obj;
    std::function<void()> callback;
    std::chrono::steady_clock::time_point when;
    const void* token;
    Handler* target;
    
    static Message* obtain();
    void recycle();
    
private:
    static std::queue<Message*> sPool;
    static std::mutex sPoolMutex;
}; 