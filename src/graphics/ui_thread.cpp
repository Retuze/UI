#include "graphics/ui_thread.h"
#include "core/looper.h"
#include "core/choreographer.h"
#include "application/application.h"
#include "core/logger.h"
#include <chrono>

LOG_TAG("UIThread");

UIThread& UIThread::getInstance() {
    static UIThread instance;
    return instance;
}

void UIThread::initialize() {
    renderingPaused = false;
    running = true;
    // 使用条件变量确保UI线程完全初始化
    std::mutex initMutex;
    std::condition_variable initCv;
    bool initialized = false;

    thread = std::thread([this, &initMutex, &initCv, &initialized]() {
        // 1. 在UI线程中初始化Looper
        Looper::prepare();
        
        // 2. 创建UI线程的Handler
        uiHandler = std::make_shared<Handler>(Looper::getForThread());
        uiThreadId = std::this_thread::get_id();
        
        // 3. 初始化Choreographer
        auto& choreographer = Choreographer::getInstance();
        choreographer.setFrameRate(60);  // 设置60fps
        choreographer.setFrameCallback([this](int64_t frameTimeNanos) {
            if (!renderingPaused) {
                if (Application::getInstance().getCurrentActivity()) {
                    Application::getInstance().render();
                }
            }
        });
        choreographer.start();  // 启动VSync模拟
        
        // 4. 通知初始化完成
        {
            std::lock_guard<std::mutex> lock(initMutex);
            initialized = true;
            initCv.notify_one();
        }
        
        LOGI("UI Thread initialized, first frame posted");
        
        // 6. 开始消息循环
        threadLoop();
    });

    // 等待UI线程初始化完成
    std::unique_lock<std::mutex> lock(initMutex);
    initCv.wait(lock, [&initialized] { return initialized; });
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
    LOGI("UI Thread loop started");
    while (running) {
        Looper::loop();
    }
}

void UIThread::quit() {
    if (running) {
        LOGI("UIThread quitting...");
        
        // 1. 先停止渲染
        renderingPaused = true;
        
        // 2. 在UI线程中停止 Choreographer
        runOnUiThread([this]() {
            LOGI("Stopping Choreographer...");
            auto& choreographer = Choreographer::getInstance();
            choreographer.stop();
            choreographer.setFrameCallback(nullptr);
            
            // 3. 清理消息队列
            LOGI("Cleaning message queue...");
            if (uiHandler) {
                auto& queue = *uiHandler->looper->getQueue();
                queue.removeMessagesForHandler(uiHandler.get());
                queue.quit();
            }
            
            // 4. 停止消息循环
            LOGI("Stopping message loop...");
            running = false;
        });
        
        if (thread.joinable()) {
            LOGI("Waiting for UI thread to join...");
            thread.join();
        }
        
        uiHandler.reset();
        LOGI("UI Thread quit successfully");
    }
}

void UIThread::pauseRendering() {
    if (uiHandler) {
        // 先设置标志位
        renderingPaused = true;
        
        // 在UI线程清理消息队列
        runOnUiThread([this]() {
            if (uiHandler) {
                auto& queue = *uiHandler->looper->getQueue();
                queue.removeMessagesForHandler(uiHandler.get());
            }
        });
        
        // 等待当前帧渲染完成
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void UIThread::resumeRendering() {
    if (uiHandler) {
        renderingPaused = false;
        auto& choreographer = Choreographer::getInstance();
        choreographer.start();  // 重新启动VSync模拟
        LOGI("Rendering resumed");
    }
}