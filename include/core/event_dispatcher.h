#pragma once

#include "core/event.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

class EventDispatcher {
public:
    static EventDispatcher& getInstance() {
        static EventDispatcher instance;
        return instance;
    }

    // 注册事件监听器
    uint32_t addEventListener(EventType type, EventCallback callback) {
        std::lock_guard<std::mutex> lock(mutex);
        uint32_t id = nextListenerId++;
        listeners[type].push_back({id, callback});
        return id;
    }

    // 移除事件监听器
    void removeEventListener(EventType type, uint32_t id) {
        std::lock_guard<std::mutex> lock(mutex);
        auto& typeListeners = listeners[type];
        auto it = std::find_if(typeListeners.begin(), typeListeners.end(),
            [id](const auto& pair) { return pair.first == id; });
        if (it != typeListeners.end()) {
            typeListeners.erase(it);
        }
    }

    // 分发事件
    void dispatchEvent(Event& event) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = listeners.find(event.type);
        if (it != listeners.end()) {
            for (const auto& [id, callback] : it->second) {
                if (!event.handled) {
                    callback(event);
                }
            }
        }
    }

private:
    EventDispatcher() = default;
    std::mutex mutex;
    uint32_t nextListenerId = 1;
    std::unordered_map<EventType, std::vector<std::pair<uint32_t, EventCallback>>> listeners;
}; 