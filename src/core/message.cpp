#include "core/message.h"
#include "core/handler.h"
#include <chrono>
#include <thread>

// Message Pool 实现
namespace {
    std::vector<std::unique_ptr<Message>> messagePool;
    std::mutex poolMutex;
    constexpr size_t MAX_POOL_SIZE = 50;
}

std::unique_ptr<Message> Message::obtain() {
    std::lock_guard<std::mutex> lock(poolMutex);
    if (messagePool.empty()) {
        return std::make_unique<Message>();
    }
    auto msg = std::move(messagePool.back());
    messagePool.pop_back();
    return msg;
}

void Message::recycle(std::unique_ptr<Message> msg) {
    if (!msg) return;
    msg->reset();
    std::lock_guard<std::mutex> lock(poolMutex);
    if (messagePool.size() < MAX_POOL_SIZE) {
        messagePool.push_back(std::move(msg));
    }
}

// MessageQueue 实现
void MessageQueue::post(std::function<void()> message) {
    auto msg = Message::obtain();
    msg->callback = std::move(message);
    msg->when = getCurrentTime();  // 立即执行
    messageQueue.push(std::move(msg));
    messageAvailable.notify_one();
}

void MessageQueue::postDelayed(std::function<void()> message, int64_t delayMillis) {
    auto msg = Message::obtain();
    msg->callback = std::move(message);
    msg->when = getCurrentTime() + delayMillis;  // 延迟时间
    messageQueue.push(std::move(msg));
    messageAvailable.notify_one();
}

void MessageQueue::enqueueMessage(std::unique_ptr<Message> msg, Handler* handler) {
    std::lock_guard<std::mutex> lock(mutex);
    msg->target = handler;
    msg->when = getCurrentTime() + msg->when;  // 转换为绝对时间
    messageQueue.push(std::move(msg));
    messageAvailable.notify_one();
}

void MessageQueue::processNextMessage() {
    std::unique_lock<std::mutex> lock(mutex);
    
    while (!quitting) {
        if (!messageQueue.empty()) {
            auto& msg = messageQueue.top();
            auto now = getCurrentTime();
            
            if (msg->when <= now) {
                // 取出消息
                auto message = std::move(const_cast<std::unique_ptr<Message>&>(messageQueue.top()));
                messageQueue.pop();
                
                lock.unlock();
                
                // 执行消息
                if (message->callback) {
                    message->callback();
                } else if (message->target) {
                    message->target->handleMessage(*message);
                }
                
                // 回收消息到消息池
                Message::recycle(std::move(message));
                
                lock.lock();
                continue;
            }
            
            // 有消息但还没到执行时间，等待到执行时间
            messageAvailable.wait_until(lock, 
                std::chrono::steady_clock::now() + 
                std::chrono::milliseconds(msg->when - now));
            continue;
        }
        
        // 队列为空，执行空闲处理
        bool keepWaiting = true;
        for (auto it = idleHandlers.begin(); it != idleHandlers.end();) {
            bool keep = (*it)();
            if (!keep) {
                it = idleHandlers.erase(it);
            } else {
                ++it;
            }
            keepWaiting &= keep;
        }
        
        if (keepWaiting && !quitting) {
            messageAvailable.wait(lock);
        }
    }
}

void MessageQueue::removeMessagesForHandler(Handler* handler) {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::unique_ptr<Message>> temp;
    
    while (!messageQueue.empty()) {
        auto msg = std::move(const_cast<std::unique_ptr<Message>&>(messageQueue.top()));
        messageQueue.pop();
        
        if (msg->target != handler) {
            temp.push_back(std::move(msg));
        } else {
            Message::recycle(std::move(msg));
        }
    }
    
    for (auto& msg : temp) {
        messageQueue.push(std::move(msg));
    }
}

void MessageQueue::quit() {
    std::lock_guard<std::mutex> lock(mutex);
    quitting = true;
    messageAvailable.notify_all();
}

void MessageQueue::removeAllMessages() {
    std::lock_guard<std::mutex> lock(mutex);
    while (!messageQueue.empty()) {
        auto msg = std::move(const_cast<std::unique_ptr<Message>&>(messageQueue.top()));
        messageQueue.pop();
        Message::recycle(std::move(msg));
    }
}

void MessageQueue::addIdleHandler(IdleHandler handler) {
    std::lock_guard<std::mutex> lock(mutex);
    idleHandlers.push_back(std::move(handler));
}

void MessageQueue::removeIdleHandler(const IdleHandler& handler) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::remove_if(idleHandlers.begin(), idleHandlers.end(),
        [&handler](const auto& h) {
            return h.target_type() == handler.target_type();
        });
    idleHandlers.erase(it, idleHandlers.end());
} 