#pragma once
#include "widgets/widget.h"
#include <string>
#include <functional>

class Button : public Widget {
public:
    explicit Button(const std::string& text = "");
    
    void draw(Renderer* renderer) override;
    
    // 删除 handleEvent 的声明，改为重写具体的事件处理方法
    bool onMousePress(const Event& event) override;
    bool onMouseRelease(const Event& event) override;
    bool onMouseMove(const Event& event) override;
    
    // 设置按钮属性
    void setText(const std::string& text);
    void setTextColor(const Color& color);
    void setBackgroundColor(const Color& color);
    void setHoverColor(const Color& color);
    void setPressedColor(const Color& color);
    
    // 事件回调
    void setOnClickListener(std::function<void()> listener);
    
    // Button 通常不需要重写 dispatchEvent，使用基类实现
    
    // Button 通常也不需要拦截事件
    bool onInterceptEvent(const Event& event) override {
        return false;
    }
    
    
protected:
    std::string text;
    Color textColor{0, 0, 0};         // 文字颜色
    Color backgroundColor{220, 220, 220}; // 普通状态背景色
    Color hoverColor{230, 230, 230};     // 悬停状态背景色
    Color pressedColor{200, 200, 200};   // 按下状态背景色
    
    bool isHovered = false;
    bool isPressed = false;
    std::function<void()> onClick;
}; 