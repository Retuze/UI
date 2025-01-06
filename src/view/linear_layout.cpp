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
    int totalWeight = 0;
    int totalHeight = 0;
    int maxWidth = 0;
    int childState = 0;
    int alternativeMaxWidth = 0;
    bool matchWidth = false;
    
    // 第一次遍历：测量非weight子视图
    for (auto* child : children) {
        if (!child->isVisible()) continue;
        
        auto& params = child->getLayoutParams();
        totalWeight += params.weight;
        
        if (params.weight == 0) {
            measureChild(child, widthMeasureSpec, heightMeasureSpec);
            totalHeight += child->getMeasuredHeight() + params.marginTop + params.marginBottom;
            maxWidth = std::max(maxWidth, 
                child->getMeasuredWidth() + params.marginLeft + params.marginRight);
        } else if (params.height == 0) {
            // 跳过weight>0且height=0的视图
            continue;
        }
        
        if (params.width == LayoutParams::MATCH_PARENT) {
            matchWidth = true;
        }
    }
    
    // 考虑padding
    totalHeight += paddingTop + paddingBottom;
    maxWidth += paddingLeft + paddingRight;
    
    // 计算剩余空间
    int remainingSpace = MeasureSpec::getSize(heightMeasureSpec) - totalHeight;
    
    // 第二次遍历：测量weight子视图
    if (totalWeight > 0 && remainingSpace > 0) {
        for (auto* child : children) {
            if (!child->isVisible()) continue;
            
            auto& params = child->getLayoutParams();
            if (params.weight > 0) {
                int shareHeight = static_cast<int>(remainingSpace * params.weight / totalWeight);
                
                int childHeightSpec = MeasureSpec::makeMeasureSpec(
                    shareHeight, MeasureSpec::EXACTLY);
                int childWidthSpec = getChildMeasureSpec(widthMeasureSpec,
                    paddingLeft + paddingRight + params.marginLeft + params.marginRight,
                    params.width);
                    
                child->measure(childWidthSpec, childHeightSpec);
                
                maxWidth = std::max(maxWidth,
                    child->getMeasuredWidth() + params.marginLeft + params.marginRight);
            }
        }
    }
    
    // 设置自己的测量尺寸
    int widthSize = MeasureSpec::getSize(widthMeasureSpec);
    int heightSize = MeasureSpec::getSize(heightMeasureSpec);
    
    bool widthExactly = MeasureSpec::getMode(widthMeasureSpec) == MeasureSpec::EXACTLY;
    bool heightExactly = MeasureSpec::getMode(heightMeasureSpec) == MeasureSpec::EXACTLY;
    
    int width = widthExactly ? widthSize : maxWidth;
    int height = heightExactly ? heightSize : totalHeight;
    
    setMeasuredDimension(width, height);
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
    int paddedLeft = l + paddingLeft;
    int paddedRight = r - paddingRight;
    int paddedTop = t + paddingTop;
    int contentWidth = r - l - paddingLeft - paddingRight;
    
    int childTop = paddedTop;
    
    for (auto* child : children) {
        if (!child->isVisible()) continue;
        
        auto& params = child->getLayoutParams();
        int childWidth = child->getMeasuredWidth();
        int childHeight = child->getMeasuredHeight();
        
        // 处理水平方向的gravity
        int childLeft = paddedLeft + params.marginLeft;
        if (childWidth < contentWidth) {
            switch (gravity & Gravity::HORIZONTAL_GRAVITY_MASK) {
                case Gravity::Center:
                    childLeft = paddedLeft + (contentWidth - childWidth) / 2;
                    break;
                case Gravity::Right:
                    childLeft = paddedRight - childWidth - params.marginRight;
                    break;
            }
        }
        
        childTop += params.marginTop;
        child->layout(childLeft, childTop, 
                     childLeft + childWidth, 
                     childTop + childHeight);
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

int LinearLayout::getChildMeasureSpec(int spec, int padding, int childDimension) {
    return ViewGroup::getChildMeasureSpec(spec, padding, childDimension);
} 