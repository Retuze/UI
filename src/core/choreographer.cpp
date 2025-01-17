#include "core/choreographer.h"
#include "graphics/surface.h"
#include <chrono>
#include <thread>
#include <stdexcept>
#include <mutex>

Choreographer& Choreographer::getInstance() {
    static Choreographer instance;
    return instance;
}

Choreographer::Choreographer() = default;

void Choreographer::setSurface(Surface* surface) {
    this->surface = surface;
}

void Choreographer::postFrameCallback(FrameCallback callback) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    frameCallbacks.push(std::move(callback));
}

bool Choreographer::start() {
    if (running.exchange(true)) {
        return false;
    }
    
    if (!surface) {
        running = false;
        return false;
    }
    
    vsyncThread = std::make_unique<std::thread>(&Choreographer::vsyncLoop, this);
    return true;
}

void Choreographer::stop() {
    if (running.exchange(false)) {
        if (vsyncThread && vsyncThread->joinable()) {
            vsyncThread->join();
        }
    }
}

void Choreographer::setFallbackFrameRate(int fps) {
    if (fps < 1 || fps > 240) {
        throw std::invalid_argument("FPS must be between 1 and 240");
    }
    fallbackFrameIntervalNanos = static_cast<int64_t>(1000000000.0 / fps);
}

void Choreographer::vsyncLoop() {
    using namespace std::chrono;
    bool vsyncEnabled = surface->isVSyncEnabled();
    
    while (running) {
        auto frameStart = high_resolution_clock::now();
        
        if (vsyncEnabled) {
            surface->waitVSync();
        } else {
            auto frameEnd = high_resolution_clock::now();
            auto frameDuration = duration_cast<nanoseconds>(frameEnd - frameStart);
            auto sleepTime = nanoseconds(fallbackFrameIntervalNanos) - frameDuration;
            
            if (sleepTime > nanoseconds::zero()) {
                std::this_thread::sleep_for(sleepTime);
            }
        }
        
        int64_t frameTimeNanos = duration_cast<nanoseconds>(
            frameStart.time_since_epoch()).count();
        executeCallbacks(frameTimeNanos);
    }
}

void Choreographer::executeCallbacks(int64_t frameTimeNanos) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    while (!frameCallbacks.empty()) {
        auto callback = std::move(frameCallbacks.front());
        frameCallbacks.pop();
        callback(frameTimeNanos);
    }
}
