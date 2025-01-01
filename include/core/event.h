#pragma once
#include "core/types.h"
#include <functional>
#include <vector>

enum class EventType {
    None,
    MousePress,
    MouseRelease,
    MouseMove,
    KeyPress,
    KeyRelease,
    Quit
};

struct Event {
    EventType type = EventType::None;
    int x = 0;
    int y = 0;
    int key = 0;
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