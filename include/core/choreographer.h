#pragma once
#include <functional>
#include <memory>
#include "core/handler.h"

class Choreographer {
public:
    static Choreographer& getInstance();
    
    void setFrameCallback(std::function<void()> callback);
    void postFrameCallback();
    
private:
    Choreographer() = default;
    std::function<void()> frameCallback;
    std::shared_ptr<Handler> handler;
}; 