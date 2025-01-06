#pragma once
#include "view/view_group.h"
#include "view/gravity.h"

class LinearLayout : public ViewGroup {
public:
    enum class Orientation {
        Horizontal,
        Vertical
    };
    
    explicit LinearLayout(Orientation orientation = Orientation::Vertical);
    
    void setOrientation(Orientation orientation);
    void setGravity(int gravity);
    
protected:
    void onMeasure(int widthMeasureSpec, int heightMeasureSpec) override;
    void onLayout(bool changed, int l, int t, int r, int b) override;
    
private:
    Orientation orientation = Orientation::Vertical;
    int gravity = 0;
    
    int getChildMeasureSpec(int spec, int padding, int childDimension);
    void measureVertical(int widthMeasureSpec, int heightMeasureSpec);
    void measureHorizontal(int widthMeasureSpec, int heightMeasureSpec);
    void layoutVertical(bool changed, int l, int t, int r, int b);
    void layoutHorizontal(bool changed, int l, int t, int r, int b);
}; 