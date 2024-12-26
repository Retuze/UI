#pragma once
#include <functional>

enum class EventType {
    MouseMove,
    MousePress,
    MouseRelease,
    KeyPress,
    KeyRelease
};

struct Event {
    EventType type;
    union {
        struct { int x, y; } mousePos;
        struct { int keyCode; } key;
    } data;
};

class EventDispatcher {
public:
    using EventHandler = std::function<bool(const Event&)>;
    
    void addEventListener(EventType type, EventHandler handler);
    bool dispatchEvent(const Event& event);

private:
    std::vector<std::pair<EventType, EventHandler>> handlers;
}; 