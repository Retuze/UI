#include "layout/linear_layout.h"

LinearLayout::LinearLayout(Orientation orientation)
    : orientation(orientation), spacing(0) {}

void LinearLayout::arrange(const std::vector<Widget*>& widgets) {
    if (orientation == Orientation::Horizontal) {
        arrangeHorizontal(widgets);
    } else {
        arrangeVertical(widgets);
    }
}

void LinearLayout::arrangeHorizontal(const std::vector<Widget*>& widgets) {
    // 计算总权重和固定宽度
    float totalWeight = 0;
    int totalFixed = 0;
    int visibleCount = 0;
    
    for (auto* widget : widgets) {
        if (!widget->isVisible()) continue;
        visibleCount++;
        
        auto it = constraints.find(widget);
        if (it != constraints.end() && it->second.weightX > 0) {
            totalWeight += it->second.weightX;
        } else {
            totalFixed += widget->getBounds().width;
        }
    }
    
    // 计算每单位权重对应的像素
    int remainingSpace = bounds.width - totalFixed - (spacing * (visibleCount - 1));
    float pixelsPerWeight = (totalWeight > 0) ? remainingSpace / totalWeight : 0;
    
    // 排列控件
    int currentX = bounds.x;
    for (auto* widget : widgets) {
        if (!widget->isVisible()) continue;
        
        Rect widgetBounds = widget->getBounds();
        auto it = constraints.find(widget);
        
        // 计算宽度
        if (it != constraints.end() && it->second.weightX > 0) {
            widgetBounds.width = static_cast<int>(it->second.weightX * pixelsPerWeight);
        }
        
        // 计算垂直对齐
        switch (alignment) {
            case Alignment::Start:
                widgetBounds.y = bounds.y;
                break;
            case Alignment::Center:
                widgetBounds.y = bounds.y + (bounds.height - widgetBounds.height) / 2;
                break;
            case Alignment::End:
                widgetBounds.y = bounds.y + bounds.height - widgetBounds.height;
                break;
            case Alignment::Stretch:
                widgetBounds.y = bounds.y;
                widgetBounds.height = bounds.height;
                break;
        }
        
        widgetBounds.x = currentX;
        widget->setBounds(widgetBounds);
        currentX += widgetBounds.width + spacing;
    }
}

void LinearLayout::arrangeVertical(const std::vector<Widget*>& widgets) {
    // 计算总权重和固定高度
    float totalWeight = 0;
    int totalFixed = 0;
    int visibleCount = 0;
    
    for (auto* widget : widgets) {
        if (!widget->isVisible()) continue;
        visibleCount++;
        
        auto it = constraints.find(widget);
        if (it != constraints.end() && it->second.weightY > 0) {
            totalWeight += it->second.weightY;
        } else {
            totalFixed += widget->getBounds().height;
        }
    }
    
    // 计算每单位权重对应的像素
    int remainingSpace = bounds.height - totalFixed - (spacing * (visibleCount - 1));
    float pixelsPerWeight = (totalWeight > 0) ? remainingSpace / totalWeight : 0;
    
    // 排列控件
    int currentY = bounds.y;
    for (auto* widget : widgets) {
        if (!widget->isVisible()) continue;
        
        Rect widgetBounds = widget->getBounds();
        auto it = constraints.find(widget);
        
        // 计算高度
        if (it != constraints.end() && it->second.weightY > 0) {
            widgetBounds.height = static_cast<int>(it->second.weightY * pixelsPerWeight);
        }
        
        // 计算水平对齐
        switch (alignment) {
            case Alignment::Start:
                widgetBounds.x = bounds.x;
                break;
            case Alignment::Center:
                widgetBounds.x = bounds.x + (bounds.width - widgetBounds.width) / 2;
                break;
            case Alignment::End:
                widgetBounds.x = bounds.x + bounds.width - widgetBounds.width;
                break;
            case Alignment::Stretch:
                widgetBounds.x = bounds.x;
                widgetBounds.width = bounds.width;
                break;
        }
        
        widgetBounds.y = currentY;
        widget->setBounds(widgetBounds);
        currentY += widgetBounds.height + spacing;
    }
}

void LinearLayout::setAlignment(Alignment alignment) {
    this->alignment = alignment;
}

void LinearLayout::setChildConstraints(Widget* widget, const LayoutConstraints& constraints) {
    this->constraints[widget] = constraints;
}

Size LinearLayout::calculatePreferredSize() {
    Size size;
    // TODO: 计算首选大小
    return size;
}

void LinearLayout::setOrientation(Orientation orientation) {
    this->orientation = orientation;
}

void LinearLayout::setSpacing(int spacing) {
    this->spacing = spacing;
} 