#pragma once
#include "widgets/widget.h"
#include <map>

struct LayoutConstraints {
    int minWidth = 0;
    int minHeight = 0;
    int maxWidth = INT_MAX;
    int maxHeight = INT_MAX;
    float weightX = 0;
    float weightY = 0;
};

class Layout {
public:
    virtual ~Layout() = default;
    virtual void arrange(const std::vector<Widget*>& widgets) = 0;
    virtual Size calculatePreferredSize() = 0;
    
    void setBounds(const Rect& bounds) { this->bounds = bounds; }

protected:
    Rect bounds;
    std::map<Widget*, LayoutConstraints> constraints;
}; 