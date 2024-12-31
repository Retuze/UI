#pragma once
#include "widgets/text_view.h"

class Button : public TextView {
public:
    Button();
    explicit Button(const std::string& text);
    
    // 事件处理
    bool onEvent(const Event& event) override;
    void onDraw(RenderContext& context) override;
    
    // 点击事件回调
    using OnClickListener = std::function<void()>;
    void setOnClickListener(OnClickListener listener);
    
protected:
    OnClickListener clickListener;
    bool pressed = false;
    Color normalColor = Color(0xE0E0E0FF);
    Color pressedColor = Color(0xBDBDBDFF);
}; 