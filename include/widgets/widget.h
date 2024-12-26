#pragma once
#include "core/types.h"
#include "core/event.h"
#include "graphics/renderer.h"
#include <vector>

class Widget {
public:
    Widget();
    virtual ~Widget() = default;
    
    virtual void draw(Renderer* renderer) = 0;
    virtual bool handleEvent(const Event& event) = 0;
    
    void setPosition(int x, int y);
    void setSize(int width, int height);
    void setBounds(const Rect& bounds);
    
    const Rect& getBounds() const { return bounds; }
    Widget* getParent() const { return parent; }
    
    void addChild(Widget* child);
    void removeChild(Widget* child);
    
    // 事件处理
    virtual bool dispatchEvent(const Event& event);
    virtual bool onEvent(const Event& event);
    
    // 焦点管理
    bool hasFocus() const { return focused; }
    virtual void setFocus(bool focus);
    
    // 事件拦截：决定是否拦截事件不给子控件
    virtual bool onInterceptEvent(const Event& event) {
        return false;  // 默认不拦截
    }

protected:
    Rect bounds;
    Widget* parent;
    std::vector<Widget*> children;
    bool visible;
    bool focused = false;
    
    // 事件处理辅助方法
    virtual bool onMousePress(const Event& event);
    virtual bool onMouseRelease(const Event& event);
    virtual bool onMouseMove(const Event& event);
}; 