#include "graphics/ui_thread.h"
#include "core/looper.h"
#include "core/choreographer.h"

UIThread& UIThread::getInstance() {
    static UIThread instance;
    return instance;
}

void UIThread::initialize() {
    Looper::prepare();
    
    // 创建UI Handler
    uiHandler = std::make_unique<Handler>(Looper::getForThread());
    
    // 保存UI线程ID
    uiThreadId = std::this_thread::get_id();
    
    // 启动UI线程
    running = true;
    thread = std::thread(&UIThread::threadLoop, this);
}

bool UIThread::isUiThread() const {
    return std::this_thread::get_id() == uiThreadId;
}

void UIThread::runOnUiThread(std::function<void()> task) {
    if (isUiThread()) {
        task();
    } else {
        uiHandler->post(std::move(task));
    }
}

void UIThread::runOnUiThreadDelayed(std::function<void()> task, int64_t delayMillis) {
    uiHandler->postDelayed(std::move(task), delayMillis);
}

void UIThread::threadLoop() {
    while (running) {
        Looper::loop();
    }
}

void UIThread::quit() {
    if (running) {
        running = false;
        if (thread.joinable()) {
            thread.join();
        }
    }
} 