#include "core/choreographer.h"
#include "graphics/ui_thread.h"
#include "core/logger.h"

LOG_TAG("Choreographer");

Choreographer::Choreographer() {
    // 检查DWM是否可用
    BOOL enabled = FALSE;
    dwmSupported = SUCCEEDED(DwmIsCompositionEnabled(&enabled)) && enabled;
    LOGI("DWM composition %s", dwmSupported ? "enabled" : "disabled");
}

Choreographer& Choreographer::getInstance() {
    static Choreographer instance;
    return instance;
}

void Choreographer::setFrameCallback(std::function<void(int64_t)> callback) {
    frameCallback = std::move(callback);
}

void Choreographer::start() {
    if (!running) {
        running = true;
        vsyncThread = std::make_unique<std::thread>(&Choreographer::vsyncLoop, this);
    }
}

void Choreographer::stop() {
    if (running.load(std::memory_order_acquire)) {
        LOGI("Choreographer stopping...");
        frameCallback = nullptr;
        running.store(false, std::memory_order_release);
        
        if (vsyncThread && vsyncThread->joinable()) {
            vsyncThread->join();
        }
        vsyncThread.reset();
        LOGI("Choreographer stopped successfully");
    }
}

bool Choreographer::isDwmEnabled() const {
    if (!dwmSupported) return false;
    
    BOOL enabled = FALSE;
    return SUCCEEDED(DwmIsCompositionEnabled(&enabled)) && enabled;
}

void Choreographer::vsyncLoop() {
    LOGI("VSync loop started");
    auto lastVsync = std::chrono::steady_clock::now();
    
    while (running.load(std::memory_order_acquire)) {
        if (isDwmEnabled()) {
            DwmFlush();
        } else {
            auto now = std::chrono::steady_clock::now();
            auto targetVsync = lastVsync + std::chrono::nanoseconds(frameIntervalNanos);
            if (targetVsync > now) {
                std::this_thread::sleep_until(targetVsync);
            }
        }
        
        auto currentVsync = std::chrono::steady_clock::now();
        lastVsync = currentVsync;
        
        // 重要：检查是否还在运行
        if (!running.load(std::memory_order_acquire)) {
            break;
        }
        
        // 获取回调的本地副本
        auto callback = frameCallback;
        if (callback) {
            auto frameTimeNanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
                currentVsync.time_since_epoch()).count();
            UIThread::getInstance().runOnUiThread([callback, frameTimeNanos]() {
                callback(frameTimeNanos);
            });
        }
    }
    LOGI("VSync loop exited");
} 