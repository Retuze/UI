#include "core/choreographer.h"
#include "graphics/ui_thread.h"

Choreographer& Choreographer::getInstance() {
    static Choreographer instance;
    return instance;
}

void Choreographer::setFrameCallback(std::function<void()> callback) {
    frameCallback = std::move(callback);
    handler = std::make_shared<Handler>(Looper::getForThread());
}

void Choreographer::postFrameCallback() {
    if (frameCallback && handler) {
        handler->post(frameCallback);
    }
} 