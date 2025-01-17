#pragma once
#include "graphics/render_context.h"
#include "graphics/ui_thread.h"
#include "view/view.h"

class ViewRoot {
public:
    ViewRoot();
    
    void setView(View* view);
    void setSurface(Surface* surface) { mainSurface = surface; }
    void setRenderContext(RenderContext* context) { renderContext = context; }
    
    void requestLayout();
    void performTraversals();
    void invalidate();

private:
    void performMeasure();
    void performLayout();
    void performDraw();
    
    View* hostView = nullptr;
    Surface* mainSurface = nullptr;
    bool layoutRequested = false;
    bool needsRedraw = false;
    RenderContext* renderContext = nullptr;
};