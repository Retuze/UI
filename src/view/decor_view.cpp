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
        auto& params = contentView->getLayoutParams();
        
        // 处理宽度
        int childWidthSpec = ViewGroup::getChildMeasureSpec(widthMeasureSpec,
            paddingLeft + paddingRight,
            params.width);
            
        // 处理高度
        int childHeightSpec = getChildMeasureSpec(heightMeasureSpec,
            paddingTop + paddingBottom,
            params.height);
            
        contentView->measure(childWidthSpec, childHeightSpec);
    }
    
    int width = MeasureSpec::getSize(widthMeasureSpec);
    int height = MeasureSpec::getSize(heightMeasureSpec);
    
    setMeasuredDimension(width, height);
}

void DecorView::onLayout(bool changed, int left, int top, int right, int bottom) {
    if (contentView) {
        // 布局内容视图以填充整个DecorView
        contentView->layout(0, 0, right - left, bottom - top);
    }
}

int DecorView::getChildMeasureSpec(int spec, int padding, int childDimension) {
    return ViewGroup::getChildMeasureSpec(spec, padding, childDimension);
} 