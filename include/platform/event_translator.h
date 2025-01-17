#pragma once

#include "core/event.h"
#include <windows.h>

class EventTranslator {
public:
    virtual ~EventTranslator() = default;
    virtual bool translateNativeEvent(void* nativeEvent, Event& outEvent) = 0;
};

#ifdef _WIN32
class Win32EventTranslator : public EventTranslator {
public:
    bool translateNativeEvent(void* nativeEvent, Event& outEvent) override {
        MSG* msg = static_cast<MSG*>(nativeEvent);
        // 转换Windows消息为标准事件
        // 这里可以参考Win32Surface中的pollEvent方法实现
        // 行272-363的逻辑可以移到这里
        return true;
    }
};
#endif 