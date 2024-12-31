#pragma once
#include "core/types.h"
#include <functional>
#include <vector>

enum class EventType {
    Unknown,
    MousePress,
    MouseRelease,
    MouseMove,
    KeyPress,
    KeyRelease
};

struct Event {
    Event() = default;
    Event(EventType type) : type(type) {}
    
    EventType type;
    Point position;
    int button = 0;
};

using EventHandler = std::function<bool(const Event&)>;

class EventDispatcher {
public:
    void addEventListener(EventType type, EventHandler handler);
    bool dispatchEvent(const Event& event);
private:
    std::vector<std::pair<EventType, EventHandler>> handlers;
}; 