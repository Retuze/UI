#pragma once
#include "view/view.h"
#include "view/view_root.h"
#include "graphics/render_context.h"

class WindowManager {
public:
    WindowManager() = default;
    ~WindowManager();
    // 窗口操作
    virtual void addView(View* view, const LayoutParams& params);
    virtual void updateViewLayout(View* view, const LayoutParams& params);
    virtual void removeView(View* view);
    
    // 渲染相关
    void setRenderContext(RenderContext* context) { renderContext = context; }
    void performTraversals();
    void setEnabled(bool enabled) { isEnabled = enabled; }

    ViewRoot* getViewRoot() const { return viewRoot.get(); }
    
private:
    bool isEnabled = true;
    View* rootView = nullptr;           // DecorView作为根视图
    std::unique_ptr<ViewRoot> viewRoot; // ViewRoot管理视图树
    RenderContext* renderContext = nullptr;
}; 