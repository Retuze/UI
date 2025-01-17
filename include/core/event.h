#pragma once

#include <cstdint>
#include <functional>

// 事件类型枚举
enum class EventType {
    None,
    // 窗口事件
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,
    
    // 键盘事件
    KeyPressed,
    KeyReleased,
    KeyTyped,
    
    // 鼠标事件
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled,
    
    // 触摸事件
    TouchBegan,
    TouchMoved,
    TouchEnded
};

// 基础事件类
class Event {
public:
    EventType type = EventType::None;
    bool handled = false;
    
    virtual ~Event() = default;
};

// 事件分发器的回调函数类型
using EventCallback = std::function<void(Event&)>;