#include "widgets/button.h"
#include "core/logger.h"

LOG_TAG("Button");

Button::Button() : Button("") {}

Button::Button(const std::string& text) : TextView(text) {
    setTextAlignment(TextAlignment::Center);
    padding = 16.0f;  // 按钮默认使用更大的内边距
}

void Button::setOnClickListener(OnClickListener listener) {
    clickListener = std::move(listener);
}

bool Button::onEvent(const Event& event) {
    switch (event.type) {
        case EventType::MousePress:
            pressed = true;
            invalidate();
            return true;
            
        case EventType::MouseRelease:
            if (pressed) {
                pressed = false;
                invalidate();
                if (clickListener) {
                    clickListener();
                }
            }
            return true;
            
        case EventType::MouseMove:
            if (pressed) {
                pressed = false;
                invalidate();
            }
            return true;
            
        default:
            return TextView::onEvent(event);
    }
}

void Button::onDraw(RenderContext& context) {
    // 绘制按钮背景
    Paint bgPaint;
    bgPaint.setColor(pressed ? pressedColor : normalColor);
    context.drawRoundRect(bounds, 4.0f, bgPaint);
    
    // 调用父类绘制文本
    TextView::onDraw(context);
    LOGI("Drawing button at: %d,%d size: %dx%d text: %s",
    bounds.x, bounds.y, bounds.width, bounds.height,
    getText().c_str());
} 