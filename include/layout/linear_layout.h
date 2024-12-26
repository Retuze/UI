#pragma once
#include "layout/layout.h"

class LinearLayout : public Layout {
public:
    enum Orientation {
        Horizontal,
        Vertical
    };
    
    explicit LinearLayout(Orientation orientation = Vertical);
    
    void arrange(const std::vector<Widget*>& widgets) override;
    Size calculatePreferredSize() override;
    
    void setOrientation(Orientation orientation);
    void setSpacing(int spacing);

private:
    Orientation orientation;
    int spacing;
}; 