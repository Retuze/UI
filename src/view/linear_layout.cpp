#include "view/linear_layout.h"
#include "view/measure_spec.h"
#include "core/logger.h"

LOG_TAG("LinearLayout");

LinearLayout::LinearLayout(Orientation orientation) 
    : orientation(orientation) {}

void LinearLayout::setOrientation(Orientation orientation) {
    if (this->orientation != orientation) {
        this->orientation = orientation;
        requestLayout();
    }
}

void LinearLayout::setGravity(int gravity) {
    if (this->gravity != gravity) {
        this->gravity = gravity;
        requestLayout();
    }
}

void LinearLayout::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    if (orientation == Orientation::Vertical) {
        measureVertical(widthMeasureSpec, heightMeasureSpec);
    } else {
        measureHorizontal(widthMeasureSpec, heightMeasureSpec);
    }
}

void LinearLayout::measureVertical(int widthMeasureSpec, int heightMeasureSpec) {
    int totalHeight = 0;
    int maxWidth = 0;
    
    // 测量所有子视图
    for (auto* child : children) {
        if (!child->isVisible()) continue;
        
        auto& params = child->getLayoutParams();
        
        // 计算子视图的MeasureSpec
        int childWidthSpec = MeasureSpec::makeMeasureSpec(
            params.width == LayoutParams::MATCH_PARENT ? 
                MeasureSpec::getSize(widthMeasureSpec) - paddingLeft - paddingRight : 
                params.width == LayoutParams::WRAP_CONTENT ?
                    MeasureSpec::getSize(widthMeasureSpec) - paddingLeft - paddingRight :
                    params.width,
            params.width == LayoutParams::MATCH_PARENT ? 
                MeasureSpec::EXACTLY : 
                params.width == LayoutParams::WRAP_CONTENT ?
                    MeasureSpec::AT_MOST :
                    MeasureSpec::EXACTLY);
                
        int childHeightSpec = MeasureSpec::makeMeasureSpec(
            params.height == LayoutParams::MATCH_PARENT ? 
                MeasureSpec::getSize(heightMeasureSpec) - totalHeight : 
                params.height,
            params.height == LayoutParams::MATCH_PARENT ? 
                MeasureSpec::EXACTLY : MeasureSpec::AT_MOST);
        
        // 测量子视图
        child->measure(childWidthSpec, childHeightSpec);
        
        // 更新总高度和最大宽度
        totalHeight += child->getMeasuredHeight() + params.marginTop + params.marginBottom;
        maxWidth = std::max(maxWidth, 
            child->getMeasuredWidth() + params.marginLeft + params.marginRight);
    }
    
    // 考虑padding
    totalHeight += paddingTop + paddingBottom;
    maxWidth += paddingLeft + paddingRight;
    
    // 设置自己的测量尺寸
    setMeasuredDimension(
        MeasureSpec::resolveSize(maxWidth, widthMeasureSpec),
        MeasureSpec::resolveSize(totalHeight, heightMeasureSpec)
    );
}

void LinearLayout::measureHorizontal(int widthMeasureSpec, int heightMeasureSpec) {
    int totalWidth = 0;
    int maxHeight = 0;
    
    // 测量所有子视图
    for (auto* child : children) {
        if (!child->isVisible()) continue;
        
        auto& params = child->getLayoutParams();
        
        // 计算子视图的MeasureSpec
        int childWidthSpec = MeasureSpec::makeMeasureSpec(
            params.width == LayoutParams::MATCH_PARENT ? 
                MeasureSpec::getSize(widthMeasureSpec) - totalWidth : 
                params.width,
            params.width == LayoutParams::MATCH_PARENT ? 
                MeasureSpec::EXACTLY : MeasureSpec::AT_MOST);
                
        int childHeightSpec = MeasureSpec::makeMeasureSpec(
            params.height == LayoutParams::MATCH_PARENT ? 
                MeasureSpec::getSize(heightMeasureSpec) - paddingTop - paddingBottom : 
                params.height,
            params.height == LayoutParams::MATCH_PARENT ? 
                MeasureSpec::EXACTLY : MeasureSpec::AT_MOST);
        
        // 测量子视图
        child->measure(childWidthSpec, childHeightSpec);
        
        // 更新总宽度和最大高度
        totalWidth += child->getMeasuredWidth() + params.marginLeft + params.marginRight;
        maxHeight = std::max(maxHeight, 
            child->getMeasuredHeight() + params.marginTop + params.marginBottom);
    }
    
    // 考虑padding
    totalWidth += paddingLeft + paddingRight;
    maxHeight += paddingTop + paddingBottom;
    
    // 设置自己的测量尺寸
    setMeasuredDimension(
        MeasureSpec::resolveSize(totalWidth, widthMeasureSpec),
        MeasureSpec::resolveSize(maxHeight, heightMeasureSpec)
    );
}

void LinearLayout::onLayout(bool changed, int l, int t, int r, int b) {
    if (orientation == Orientation::Vertical) {
        layoutVertical(changed, l, t, r, b);
    } else {
        layoutHorizontal(changed, l, t, r, b);
    }
}

void LinearLayout::layoutVertical(bool changed, int l, int t, int r, int b) {
    LOGI("LinearLayout::layoutVertical: l=%d, t=%d, r=%d, b=%d", l, t, r, b);
    
    int childTop = paddingTop;
    for (auto* child : children) {
        if (!child->isVisible()) continue;
        
        auto& params = child->getLayoutParams();
        int childWidth = r - l - paddingLeft - paddingRight - params.marginLeft - params.marginRight;
        int childHeight = child->getMeasuredHeight();
        
        childTop += params.marginTop;
        
        LOGI("Child layout: top=%d, height=%d, margins(t=%d,b=%d)", 
             childTop, childHeight, params.marginTop, params.marginBottom);
             
        child->layout(
            paddingLeft + params.marginLeft,
            childTop,
            paddingLeft + params.marginLeft + childWidth,
            childTop + childHeight
        );
        
        childTop += childHeight + params.marginBottom;
    }
}

void LinearLayout::layoutHorizontal(bool changed, int l, int t, int r, int b) {
    int childLeft = paddingLeft;
    int contentHeight = b - t - paddingTop - paddingBottom;
    
    for (auto* child : children) {
        if (!child->isVisible()) continue;
        
        auto& params = child->getLayoutParams();
        
        // 计算子视图顶边距
        int childTop = paddingTop + params.marginTop;
        if (params.height == LayoutParams::MATCH_PARENT) {
            // 如果是MATCH_PARENT，占满高度
            child->layout(
                childLeft + params.marginLeft,
                childTop,
                childLeft + params.marginLeft + child->getMeasuredWidth(),
                b - t - paddingBottom - params.marginBottom
            );
        } else {
            // 否则根据gravity调整垂直位置
            if (gravity & Gravity::VCenter) {
                childTop = paddingTop + (contentHeight - child->getMeasuredHeight()) / 2;
            } else if (gravity & Gravity::Bottom) {
                childTop = b - t - paddingBottom - child->getMeasuredHeight();
            }
            
            child->layout(
                childLeft + params.marginLeft,
                childTop,
                childLeft + params.marginLeft + child->getMeasuredWidth(),
                childTop + child->getMeasuredHeight()
            );
        }
        
        // 更新下一个子视图的左边距
        childLeft += params.marginLeft + child->getMeasuredWidth() + params.marginRight;
    }
} 