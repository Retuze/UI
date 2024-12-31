#pragma once
#include "core/types.h"
#include <vector>

class View;

class Layout {
public:
    virtual ~Layout() = default;
    virtual void measure(View* view, int widthMeasureSpec, int heightMeasureSpec) = 0;
    virtual void arrange(const std::vector<View*>& children) = 0;
    void setBounds(const Rect& bounds) { this->bounds = bounds; }
    
protected:
    Rect bounds;
}; 