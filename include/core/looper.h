#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

class Message;
class Handler;

class Looper {
public:
    static Looper& getInstance();
    static void prepare();
    static void loop();
    
    void postMessage(Message* msg);
    void quit();
    
    // 获取当前线程的 Looper
    static Looper* myLooper();

private:
    Looper();
    ~Looper();
    
    std::queue<Message*> messageQueue;
    std::mutex mutex;
    std::condition_variable condition;
    std::thread::id threadId;
    bool running = true;
    
    static thread_local Looper* sThreadLooper;
}; 