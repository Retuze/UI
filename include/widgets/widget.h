#pragma once
#include "core/types.h"
#include "core/event.h"
#include "graphics/renderer.h"
#include <vector>
#include <algorithm>

class Widget {
public:
    Widget();
    virtual ~Widget() = default;
    
    virtual void draw(Renderer* renderer) = 0;
    
    // 事件分发机制
    virtual bool dispatchEvent(const Event& event);  // 事件分发
    virtual bool onInterceptEvent(const Event& event) { return false; }  // 拦截判断
    virtual bool onEvent(const Event& event);  // 事件处理
    
    void setPosition(int x, int y);
    void setSize(int width, int height);
    void setBounds(const Rect& bounds);
    
    const Rect& getBounds() const { return bounds; }
    Widget* getParent() const { return parent; }
    
    void addChild(Widget* child);
    void removeChild(Widget* child);
    
    // 焦点管理
    bool hasFocus() const { return focused; }
    virtual void setFocus(bool focus);
    
    void setParent(Widget* parent) { this->parent = parent; }
    bool isVisible() const { return visible; }

protected:
    // 具体事件处理方法
    virtual bool onMousePress(const Event& event) { return false; }
    virtual bool onMouseRelease(const Event& event) { return false; }
    virtual bool onMouseMove(const Event& event) { return false; }
    
    Rect bounds;
    Widget* parent;
    std::vector<Widget*> children;
    bool visible;
    bool focused = false;
}; 