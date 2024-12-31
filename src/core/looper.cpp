#include "core/looper.h"

thread_local std::shared_ptr<Looper> Looper::threadLooper = nullptr;

std::shared_ptr<Looper> Looper::getForThread() {
    return threadLooper;
}

void Looper::prepare() {
    if (!threadLooper) {
        threadLooper = std::shared_ptr<Looper>(new Looper());
    }
}

void Looper::loop() {
    auto looper = getForThread();
    if (looper && looper->queue) {
        looper->queue->processNextMessage();
    }
}

Looper::Looper() : queue(std::make_unique<MessageQueue>()) {
} 