#include "core/event.h"

void EventDispatcher::addEventListener(EventType type, EventHandler handler) {
    handlers.emplace_back(type, std::move(handler));
}

bool EventDispatcher::dispatchEvent(const Event& event) {
    for (auto it = handlers.rbegin(); it != handlers.rend(); ++it) {
        if (it->first == event.type) {
            if (it->second(event)) {
                return true;
            }
        }
    }
    return false;
} 