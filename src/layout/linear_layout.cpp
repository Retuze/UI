#include "layout/linear_layout.h"

LinearLayout::LinearLayout(Orientation orientation)
    : orientation(orientation), spacing(0) {}

void LinearLayout::arrange(const std::vector<Widget*>& widgets) {
    int currentPos = 0;
    
    for (auto* widget : widgets) {
        if (orientation == Orientation::Horizontal) {
            widget->setPosition(currentPos, 0);
            currentPos += widget->getBounds().width + spacing;
        } else {
            widget->setPosition(0, currentPos);
            currentPos += widget->getBounds().height + spacing;
        }
    }
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