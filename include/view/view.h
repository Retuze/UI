#pragma once
#include "core/types.h"
#include "core/event.h"
#include "graphics/render_context.h"
#include <vector>
#include <memory>
#include "view/measure_spec.h"

class ViewGroup;

class View {
public:
    View();
    virtual ~View() = default;
    
    // 测量和布局
    virtual void measure(int widthMeasureSpec, int heightMeasureSpec);
    virtual void layout(int left, int top, int right, int bottom);
    
    // 绘制
    void draw(RenderContext& context);
    virtual void onDraw(RenderContext& context) = 0;
    
    // 事件处理
    virtual bool dispatchEvent(const Event& event);
    virtual bool onEvent(const Event& event);
    
    // 属性设置
    void setPosition(int x, int y);
    void setSize(int width, int height);
    void setBounds(const Rect& bounds);
    void setVisible(bool visible);
    
    // 状态查询
    const Rect& getBounds() const { return bounds; }
    ViewGroup* getParent() const { return parent; }
    bool isVisible() const { return visible; }
    
    // 无效化处理
    void invalidate();
    void requestLayout();
    
    virtual void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {}
    
protected:
    friend class ViewGroup;
    
    Rect bounds;
    ViewGroup* parent = nullptr;
    bool visible = true;
    bool needsLayout = true;
    
    // 测量结果
    int measuredWidth = 0;
    int measuredHeight = 0;
    void setMeasuredDimension(int width, int height);
}; 