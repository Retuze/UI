#include "view/decor_view.h"

DecorView::DecorView() {
    // 设置默认布局参数
    setLayoutParams({LayoutParams::MATCH_PARENT, LayoutParams::MATCH_PARENT});
}

void DecorView::setContentView(View* view) {
    // 移除旧的内容视图
    if (contentView) {
        removeView(contentView);
        contentView = nullptr;
    }
    
    // 添加新的内容视图
    if (view) {
        contentView = view;
        addView(view);
    }
}

void DecorView::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    if (contentView) {
        // 测量内容视图
        measureChild(contentView, widthMeasureSpec, heightMeasureSpec);
    }
    
    // 设置自己的测量尺寸
    setMeasuredDimension(
        MeasureSpec::getSize(widthMeasureSpec),
        MeasureSpec::getSize(heightMeasureSpec)
    );
}

void DecorView::onLayout(bool changed, int left, int top, int right, int bottom) {
    if (contentView) {
        // 布局内容视图以填充整个DecorView
        contentView->layout(0, 0, right - left, bottom - top);
    }
} 