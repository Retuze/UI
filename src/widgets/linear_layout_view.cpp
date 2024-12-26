#include "widgets/linear_layout_view.h"

LinearLayoutView::LinearLayoutView(LinearLayout::Orientation orientation) {
    auto linearLayout = new LinearLayout(orientation);
    setLayout(linearLayout);
}

void LinearLayoutView::setOrientation(LinearLayout::Orientation orientation) {
    auto* linearLayout = static_cast<LinearLayout*>(layout);
    linearLayout->setOrientation(orientation);
    requestLayout();
}

void LinearLayoutView::setAlignment(LinearLayout::Alignment alignment) {
    auto* linearLayout = static_cast<LinearLayout*>(layout);
    linearLayout->setAlignment(alignment);
    requestLayout();
}

void LinearLayoutView::setSpacing(int spacing) {
    auto* linearLayout = static_cast<LinearLayout*>(layout);
    linearLayout->setSpacing(spacing);
    requestLayout();
} 