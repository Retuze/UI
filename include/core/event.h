#pragma once
#include "core/types.h"
#include <functional>
#include <vector>

enum class EventType {
    MouseMove,
    MousePress,
    MouseRelease,
    KeyDown,
    KeyUp,
    Char,
    Quit
};

struct Event {
public:
    using Type = EventType;
    
    enum class Button {
        Left,
        Right,
        Middle
    };
    
    Type type;
    int x = 0;
    int y = 0;
    Button button;
    int keyCode = 0;
    char keyChar = 0;
};

using EventHandler = std::function<bool(const Event&)>;

class EventDispatcher {
public:
    void addEventListener(EventType type, EventHandler handler);
    bool dispatchEvent(const Event& event);
private:
    std::vector<std::pair<EventType, EventHandler>> handlers;
}; 