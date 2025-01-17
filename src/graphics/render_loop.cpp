#include "graphics/render_loop.h"
#include <chrono>
#include <cassert>

RenderLoop& RenderLoop::getInstance() {
    static RenderLoop instance;
    return instance;
}

RenderLoop::RenderLoop() 
    : isRunning(false)
    , taskHandler(nullptr) {
}

RenderLoop::~RenderLoop() {
    stop();
}

void RenderLoop::start() {
    if (isRunning) {
        return;
    }
    
    isRunning = true;
    renderThread = std::thread(&RenderLoop::run, this);
    renderThreadId = renderThread.get_id();
}

void RenderLoop::stop() {
    if (!isRunning) {
        return;
    }
    
    isRunning = false;
    if (renderThread.joinable()) {
        renderThread.join();
    }
}

void RenderLoop::post(std::function<void()> task) {
    if (!isRunning) {
        return;
    }
    taskHandler->post(std::move(task));
}

void RenderLoop::postDelayed(std::function<void()> task, int64_t delayMillis) {
    if (!isRunning) {
        return;
    }
    taskHandler->postDelayed(std::move(task), delayMillis);
}

bool RenderLoop::isOnRenderThread() const {
    return std::this_thread::get_id() == renderThreadId;
}

void RenderLoop::pauseRendering() {
    post([this]() {
        // TODO: 实现渲染暂停逻辑
    });
}

void RenderLoop::resumeRendering() {
    post([this]() {
        // TODO: 实现渲染恢复逻辑
    });
}

void RenderLoop::run() {
    // 在渲染线程中创建 looper
    Looper::prepare();
    looper = std::unique_ptr<Looper>(Looper::getForThread());
    taskHandler = std::make_unique<Handler>(looper.get());
    renderThreadId = std::this_thread::get_id();

    while (isRunning) {
        looper->loop();
        
        // 在这里可以添加渲染相关的处理逻辑
        // 比如处理渲染队列、更新画面等
        
        // 控制帧率
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60fps
    }

    // 清理资源
    taskHandler.reset();
    looper.reset();
} 