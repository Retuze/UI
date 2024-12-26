#pragma once
#include "widgets/widget.h"
#include "layout/layout.h"

class ViewGroup : public Widget {
public:
    ViewGroup();
    ~ViewGroup() override;
    
    // 子控件管理
    void addView(Widget* child);
    void removeView(Widget* child);
    void removeAllViews();
    
    // 布局相关
    void setLayout(Layout* layout);
    void requestLayout();
    
    // 绘制
    void draw(Renderer* renderer) override;
    
    // 事件处理
    bool dispatchEvent(const Event& event) override;
    virtual bool onInterceptEvent(const Event& event) override { return false; }

protected:
    Layout* layout;
    std::vector<Widget*> children;
    bool needsLayout;
    
    // 执行布局
    virtual void onLayout();
}; 