#pragma once
#include <functional>
#include <chrono>

class Message;
class Looper;

class Handler {
public:
    explicit Handler(Looper* looper = nullptr);
    
    void sendMessage(Message* msg);
    void post(std::function<void()> callback);
    void postDelayed(std::function<void()> callback, 
                    std::chrono::milliseconds delay);
    void removeCallbacks(const void* token);
    
    virtual void handleMessage(Message* msg);

protected:
    Looper* looper;
}; 