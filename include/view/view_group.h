#pragma once
#include "view/view.h"
#include "view/layout.h"
#include <vector>

class ViewGroup : public View {
public:
    ViewGroup();
    ~ViewGroup() override;
    
    // 子视图管理
    void addView(View* child);
    void removeView(View* child);
    void removeAllViews();
    
    // 布局管理
    void setLayout(Layout* layout);
    
    // 重写基类方法
    void measure(int widthMeasureSpec, int heightMeasureSpec) override;
    void layout(int left, int top, int right, int bottom) override;
    void onDraw(RenderContext& context) override;
    bool dispatchEvent(const Event& event) override;
    
protected:
    void measureChild(View* child, int parentWidthMeasureSpec, int parentHeightMeasureSpec);
    std::vector<View*> children;
    std::unique_ptr<Layout> layoutManager;
    
    // 布局相关
    virtual void onLayout(bool changed, int l, int t, int r, int b);
    virtual void onMeasure(int widthMeasureSpec, int heightMeasureSpec) override;
}; 