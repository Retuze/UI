#pragma once
#include "core/types.h"
#include "core/event.h"
#include "graphics/render_context.h"
#include <vector>
#include <memory>
#include "view/measure_spec.h"
#include "view/layout_params.h"

class ViewGroup;

class View {
public:
    View();
    virtual ~View() = default;
    
    // 布局相关
    void setLayoutParams(const LayoutParams& params);
    const LayoutParams& getLayoutParams() const { return layoutParams; }
    void setPadding(int left, int top, int right, int bottom);
    
    // 测量和布局
    virtual void measure(int widthMeasureSpec, int heightMeasureSpec);
    virtual void layout(int left, int top, int right, int bottom);
    
    // 绘制
    void draw(RenderContext& context);
    virtual void onDraw(RenderContext& context) = 0;
    
    // 事件处理
    virtual bool dispatchEvent(const Event& event);
    virtual bool onEvent(const Event& event);
    
    // Getter/Setter
    void setPosition(int x, int y);
    void setSize(int width, int height);
    void setBounds(const Rect& bounds);
    void setVisible(bool visible);
    bool isVisible() const { return visible; }
    
    // 布局请求
    void invalidate();
    void requestLayout();
    
    virtual void onMeasure(int widthMeasureSpec, int heightMeasureSpec);
    
    int getMeasuredWidth() const { return measuredWidth; }
    int getMeasuredHeight() const { return measuredHeight; }
    
    enum Visibility {
        VISIBLE,
        INVISIBLE,
        GONE
    };
    
    Visibility getVisibility() const { return visibility; }
    void setVisibility(Visibility v) { visibility = v; }
    
    // Add padding getters
    int getPaddingLeft() const { return paddingLeft; }
    int getPaddingTop() const { return paddingTop; }
    int getPaddingRight() const { return paddingRight; }
    int getPaddingBottom() const { return paddingBottom; }
    
protected:
    void setMeasuredDimension(int width, int height);
    
    LayoutParams layoutParams;
    Rect bounds;
    int measuredWidth = 0;
    int measuredHeight = 0;
    int paddingLeft = 0;
    int paddingTop = 0;
    int paddingRight = 0;
    int paddingBottom = 0;
    bool visible = true;
    bool needsLayout = true;
    ViewGroup* parent = nullptr;
    
    Visibility visibility = VISIBLE;
    
    friend class ViewGroup;
}; 