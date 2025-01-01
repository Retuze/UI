#include "core/handler.h"

Handler::~Handler() {
    if (looper && looper->getQueue()) {
        looper->getQueue()->removeMessagesForHandler(this);
    }
}

void Handler::sendMessage(std::unique_ptr<Message> msg) {
    msg->when = std::chrono::steady_clock::now().time_since_epoch().count();
    looper->getQueue()->enqueueMessage(std::move(msg), this);
}

void Handler::sendMessageDelayed(std::unique_ptr<Message> msg, int64_t delayMillis) {
    msg->when = std::chrono::steady_clock::now().time_since_epoch().count() + 
               delayMillis * 1000000;
    looper->getQueue()->enqueueMessage(std::move(msg), this);
}

void Handler::sendMessageAtTime(std::unique_ptr<Message> msg, int64_t uptimeMillis) {
    msg->when = uptimeMillis * 1000000;
    looper->getQueue()->enqueueMessage(std::move(msg), this);
}

void Handler::sendEmptyMessage(int what) {
    auto msg = Message::obtain();
    msg->what = what;
    sendMessage(std::move(msg));
}

void Handler::sendEmptyMessageDelayed(int what, int64_t delayMillis) {
    auto msg = Message::obtain();
    msg->what = what;
    sendMessageDelayed(std::move(msg), delayMillis);
}

void Handler::post(std::function<void()> r) {
    auto msg = Message::obtain();
    msg->callback = std::move(r);
    sendMessage(std::move(msg));
}

void Handler::postDelayed(std::function<void()> r, int64_t delayMillis) {
    auto msg = Message::obtain();
    msg->callback = std::move(r);
    sendMessageDelayed(std::move(msg), delayMillis);
}