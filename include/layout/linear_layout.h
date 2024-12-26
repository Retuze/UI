#pragma once
#include "layout/layout.h"

class LinearLayout : public Layout {
public:
    enum class Orientation {
        Horizontal,
        Vertical
    };
    
    enum class Alignment {
        Start,      // 左对齐/顶对齐
        Center,     // 居中对齐
        End,        // 右对齐/底对齐
        Stretch     // 拉伸填充
    };
    
    explicit LinearLayout(Orientation orientation = Orientation::Vertical);
    
    void arrange(const std::vector<Widget*>& widgets) override;
    Size calculatePreferredSize() override;
    
    void setOrientation(Orientation orientation);
    void setSpacing(int spacing);
    void setAlignment(Alignment alignment);
    
    // 设置子控件约束
    void setChildConstraints(Widget* widget, const LayoutConstraints& constraints);

private:
    Orientation orientation;
    Alignment alignment = Alignment::Start;
    int spacing;
    
    void arrangeHorizontal(const std::vector<Widget*>& widgets);
    void arrangeVertical(const std::vector<Widget*>& widgets);
}; 