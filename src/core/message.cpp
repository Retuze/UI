#include "core/message.h"
#include "core/handler.h"
#include "core/logger.h"
#include <chrono>
#include <thread>


LOG_TAG("Message");

// Message Pool 实现
namespace
{
std::vector<Message*> messagePool;
std::mutex poolMutex;
constexpr size_t MAX_POOL_SIZE = 50;

constexpr int64_t millisToNanos(int64_t ms) {
    return ms * 1000000;
}
} // namespace

Message* Message::obtain()
{
  std::lock_guard<std::mutex> lock(poolMutex);
  if (messagePool.empty()) {
    return new Message();
  }
  Message* msg = messagePool.back();
  messagePool.pop_back();
  return msg;
}

void Message::recycle(Message* msg)
{
  if (!msg)
    return;
  msg->reset();
  std::lock_guard<std::mutex> lock(poolMutex);
  if (messagePool.size() < MAX_POOL_SIZE) {
    messagePool.push_back(msg);
  } else {
    delete msg;
  }
}

void Message::clearPool()
{
  std::lock_guard<std::mutex> lock(poolMutex);
  messagePool.clear();
}

// MessageQueue 实现
void MessageQueue::post(std::function<void()> message)
{
  Message* msg = Message::obtain();
  msg->callback = std::move(message);
  msg->when = getCurrentTimeNanos();
  messageQueue.push(msg);
  messageAvailable.notify_one();
}

void MessageQueue::postDelayed(std::function<void()> message,
                               int64_t delayMillis)
{
  Message* msg = Message::obtain();
  msg->callback = std::move(message);
  msg->when = getCurrentTimeNanos() + millisToNanos(delayMillis);
  messageQueue.push(msg);
  messageAvailable.notify_one();
}

void MessageQueue::enqueueMessage(Message* msg, Handler* handler)
{
  std::lock_guard<std::mutex> lock(mutex);
  msg->target = handler;
  if (msg->when == 0) {
    msg->when = getCurrentTimeNanos();
  } else {
    msg->when = getCurrentTimeNanos() + millisToNanos(msg->when);
  }
  messageQueue.push(msg);
  messageAvailable.notify_one();
}

void MessageQueue::processNextMessage()
{
  std::unique_lock<std::mutex> lock(mutex);

  while (!quitting) {
    if (!messageQueue.empty()) {
      Message* msg = messageQueue.top();
      auto now = getCurrentTimeNanos();

      if (msg->when <= now) {
        messageQueue.pop();

        lock.unlock();
        if (msg->callback) {
          msg->callback();
        } else if (msg->target) {
          msg->target->handleMessage(*msg);
        }

        Message::recycle(msg);
        lock.lock();
        continue;
      }

      // 等待到执行时间
      auto waitTime = std::chrono::nanoseconds(msg->when - now);
      auto waitUntil = std::chrono::steady_clock::now() + waitTime;
      messageAvailable.wait_until(lock, waitUntil);
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

  // 如果是退出状态，确保清理所有剩余消息
  if (quitting) {
    while (!messageQueue.empty()) {
      Message* msg = messageQueue.top();
      messageQueue.pop();
      
      Message::recycle(msg);
    }
  }
}

void MessageQueue::removeMessagesForHandler(Handler* handler)
{
  std::lock_guard<std::mutex> lock(mutex);
  std::vector<Message*> temp;

  while (!messageQueue.empty()) {
    Message* msg = messageQueue.top();
    messageQueue.pop();

    if (msg->target != handler) {
      temp.push_back(msg);
    } else {
      Message::recycle(msg);
    }
  }

  for (Message* msg : temp) {
    messageQueue.push(msg);
  }
}

void MessageQueue::quit()
{
    LOGI("MessageQueue quitting...");
    
    {
        std::lock_guard<std::mutex> lock(mutex);
        LOGI("Setting quit flag");
        quitting = true;
    }
    
    // 通知所有等待的线程
    LOGI("Notifying waiting threads");
    messageAvailable.notify_all();
    
    // 等待所有正在处理的消息完成
    LOGI("Yielding for pending messages");
    std::this_thread::yield();
    
    // 清理剩余消息
    LOGI("Removing all remaining messages");
    removeAllMessages();
    
    LOGI("MessageQueue quit successfully");
}

void MessageQueue::removeAllMessages()
{
  std::lock_guard<std::mutex> lock(mutex);
  while (!messageQueue.empty()) {
    Message* msg = messageQueue.top();
    messageQueue.pop();
    Message::recycle(msg);
  }
}

void MessageQueue::addIdleHandler(IdleHandler handler)
{
  std::lock_guard<std::mutex> lock(mutex);
  idleHandlers.push_back(std::move(handler));
}

void MessageQueue::removeIdleHandler(const IdleHandler& handler)
{
  std::lock_guard<std::mutex> lock(mutex);
  auto it = std::remove_if(idleHandlers.begin(), idleHandlers.end(),
                           [&handler](const auto& h) {
                             return h.target_type() == handler.target_type();
                           });
  idleHandlers.erase(it, idleHandlers.end());
}

int64_t MessageQueue::getCurrentTimeNanos()
{
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
             .count() &
         0x7FFFFFFFFFFFFFFFLL;
}