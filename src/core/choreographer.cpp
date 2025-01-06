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
    if (running) {
        // 1. 先清除回调，确保不会再触发新的UI操作
        frameCallback = nullptr;
        
        // 2. 再停止线程
        running = false;
        
        // 3. 等待线程结束
        if (vsyncThread && vsyncThread->joinable()) {
            vsyncThread->join();
        }
        vsyncThread.reset();
    }
}

bool Choreographer::isDwmEnabled() const {
    if (!dwmSupported) return false;
    
    BOOL enabled = FALSE;
    return SUCCEEDED(DwmIsCompositionEnabled(&enabled)) && enabled;
}

void Choreographer::vsyncLoop() {
    auto lastVsync = std::chrono::steady_clock::now();
    
    while (running) {
        if (isDwmEnabled()) {
            // 等待下一个VSync信号
            DwmFlush();
        } else {
            // 如果DWM不可用,回退到定时器模式
            auto now = std::chrono::steady_clock::now();
            auto targetVsync = lastVsync + std::chrono::nanoseconds(frameIntervalNanos);
            if (targetVsync > now) {
                std::this_thread::sleep_until(targetVsync);
            }
        }
        
        // 记录当前时间并计算间隔
        auto currentVsync = std::chrono::steady_clock::now();
        auto frameInterval = std::chrono::duration_cast<std::chrono::nanoseconds>(
            currentVsync - lastVsync).count();
        // LOGI("Frame interval: %lldns", frameInterval);
        
        // 更新上一帧时间
        lastVsync = currentVsync;
        
        // 在UI线程执行帧回调
        if (frameCallback) {
            auto frameTimeNanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
                currentVsync.time_since_epoch()).count();
            UIThread::getInstance().runOnUiThread([this, frameTimeNanos]() {
                frameCallback(frameTimeNanos);
            });
        }
    }
} 