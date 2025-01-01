#pragma once
#include "widgets/text_view.h"
#include <functional>

class Button : public TextView {
public:
    using OnClickListener = std::function<void()>;
    
    Button();
    explicit Button(const std::string& text);
    
    void setOnClickListener(OnClickListener listener);
    
protected:
    void onDraw(RenderContext& context) override;
    bool onEvent(const Event& event) override;
    
private:
    OnClickListener clickListener;
    bool pressed = false;
    Color normalColor = Color::LightGray();
    Color pressedColor = Color::Gray();
}; 