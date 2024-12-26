#pragma once
#include "widgets/view_group.h"
#include "layout/linear_layout.h"

class LinearLayoutView : public ViewGroup {
public:
    explicit LinearLayoutView(LinearLayout::Orientation orientation = LinearLayout::Orientation::Vertical);
    
    void setOrientation(LinearLayout::Orientation orientation);
    void setAlignment(LinearLayout::Alignment alignment);
    void setSpacing(int spacing);
    
    void draw(Renderer* renderer) override { ViewGroup::draw(renderer); }
}; 