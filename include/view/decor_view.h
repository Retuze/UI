#pragma once
#include "view/view_group.h"

class DecorView : public ViewGroup {
public:
    DecorView();
    
    void setContentView(View* view);
    View* getContentView() const { return contentView; }
    
protected:
    void onMeasure(int widthMeasureSpec, int heightMeasureSpec) override;
    void onLayout(bool changed, int left, int top, int right, int bottom) override;
    
private:
    View* contentView = nullptr;
}; 