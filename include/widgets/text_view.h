#pragma once
#include "widgets/widget.h"
#include <string>

class TextView : public Widget {
public:
    explicit TextView(const std::string& text = "");
    
    void draw(Renderer* renderer) override;
    bool handleEvent(const Event& event) override { return false; }
    
    // 基本属性设置
    void setText(const std::string& text);
    void setTextColor(const Color& color);
    void setTextSize(int size);
    
    // Padding
    void setPadding(int left, int top, int right, int bottom);

protected:
    std::string text;
    Color textColor{0, 0, 0};  // 默认黑色
    int textSize = 24;         // 默认字号
    
    struct {
        int left = 0, top = 0, right = 0, bottom = 0;
    } padding;
}; 