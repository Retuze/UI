#include "core/looper.h"

thread_local Looper* Looper::threadLooper = nullptr;

Looper* Looper::getCurrentThreadLooper() {
    return threadLooper;
}

void Looper::prepare() {
    if (!threadLooper) {
        threadLooper = new Looper();
    }
}

void Looper::loop() {
    auto looper = getCurrentThreadLooper();
    if (looper && looper->queue) {
        looper->queue->processNextMessage();
    }
}

void Looper::quit() {
    auto looper = getCurrentThreadLooper();
    if (looper && looper->queue) {
        looper->queue->quit();
    }
}

Looper::Looper() {
    queue = new MessageQueue();
}

Looper::~Looper() {
    delete queue;
} 