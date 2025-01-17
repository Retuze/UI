#include "graphics/render_loop.h"
#include "core/choreographer.h"
#include "application/application.h"
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
    
    auto& choreographer = Choreographer::getInstance();
    
    choreographer.setSurface(surface);
    
    // 注册渲染回调
    choreographer.postFrameCallback([this](int64_t frameTimeNanos) {
        // 执行具体的渲染逻辑
        Application::getInstance().render();
    });
    
    // 启动 Choreographer
    choreographer.start();

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
    isPausedFlag = true;
    // 其他暂停逻辑
}

void RenderLoop::resumeRendering() {
    isPausedFlag = false;
    // 其他恢复逻辑
}

bool RenderLoop::isPaused() const {
    return isPausedFlag;
}

void RenderLoop::run() {
    // 在渲染线程中创建 looper
    Looper::prepare();
    looper = std::unique_ptr<Looper>(Looper::getCurrentThreadLooper());
    taskHandler = std::make_unique<Handler>(looper.get());
    renderThreadId = std::this_thread::get_id();

    while (isRunning) {
        looper->loop();
        // TODO: 添加渲染相关的处理逻辑--通过Choreographer
    }

    // 清理资源
    taskHandler.reset();
    looper.reset();
} 