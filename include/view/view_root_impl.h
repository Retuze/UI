#pragma once
#include "view/view.h"

class ViewRootImpl {
public:
    void setView(View* view);
    void requestLayout();
    void invalidate();
    
    // 执行完整的视图树处理
    void performTraversal();
    
    void setRenderContext(RenderContext* context) {
        renderContext = context;
    }
    
    void setWindowSize(int width, int height) {
        windowWidth = width;
        windowHeight = height;
    }
    
private:
    // 执行测量流程
    void performMeasure(int width, int height);
    // 执行布局流程
    void performLayout();
    // 执行绘制流程
    void performDraw(RenderContext& context);
    
    View* hostView = nullptr;
    bool layoutRequested = false;
    bool invalidated = false;
    RenderContext* renderContext = nullptr;
    int windowWidth = 800;
    int windowHeight = 600;
}; 