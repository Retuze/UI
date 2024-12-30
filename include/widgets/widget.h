#pragma once
#include "core/types.h"
#include "core/event.h"
#include "graphics/renderer.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <chrono>

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
    
    // 请求重绘
    virtual void invalidate() {
        needsRedraw = true;
        // 向上传递重绘请求
        if (parent) {
            parent->invalidate();
        }
    }
    
    // 检查是否需要重绘
    bool needsRedraw = false;
    
    // 投递一个稍后执行的操作
    using Runnable = std::function<void()>;
    virtual void post(Runnable&& runnable) {
        if (parent) {
            parent->post(std::move(runnable));
        }
    }
    
    // 投递一个延迟执行的操作
    virtual void postDelayed(Runnable&& runnable, std::chrono::milliseconds delay) {
        if (parent) {
            parent->postDelayed(std::move(runnable), delay);
        }
    }
    
    static Widget* getCapturedWidget() { return capturedWidget; }
    static void setCapturedWidget(Widget* widget) { capturedWidget = widget; }
    
    // 测量相关
    virtual void measure(int widthMeasureSpec, int heightMeasureSpec);
    void setMeasuredDimension(int measuredWidth, int measuredHeight);
    
    // 布局相关 
    virtual void layout(int left, int top, int right, int bottom);
    virtual void onLayout(bool changed, int left, int top, int right, int bottom);
    
    // 绘制相关
    virtual void onDraw(Renderer* renderer);
    
    int getMeasuredWidth() const { return measuredWidth; }
    int getMeasuredHeight() const { return measuredHeight; }

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
    static Widget* capturedWidget;  // 当前捕获事件的控件
    
    int measuredWidth = 0;
    int measuredHeight = 0;
}; 