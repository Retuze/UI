#include "widgets/button.h"
#include "core/logger.h"

LOG_TAG("Button");

Button::Button() : Button("") {}

Button::Button(const std::string& text) : TextView(text) {
    setTextAlignment(TextAlignment::Center);
    setPadding(16, 16, 16, 16);  // 使用View::setPadding来设置内边距
    normalColor = Color(220, 220, 220);  // 浅灰色背景
    pressedColor = Color(180, 180, 180); // 深灰色按下效果
}

void Button::setOnClickListener(OnClickListener listener) {
    clickListener = std::move(listener);
}

bool Button::onEvent(const Event& event) {
    if (event.type != EventType::MouseMove) {
        LOGI("Button::onEvent - type=%d, pos=(%d, %d)", 
             static_cast<int>(event.type), event.x, event.y);
    }
    
    if (event.x < 0 || event.x >= bounds.width ||
        event.y < 0 || event.y >= bounds.height) {
        if (event.type != EventType::MouseMove) {
            LOGI("Event outside button bounds: bounds(w=%d,h=%d)", 
                 bounds.width, bounds.height);
        }
        if (pressed) {
            pressed = false;
            invalidate();
        }
        return false;
    }
    
    switch (event.type) {
        case EventType::MousePress:
            LOGI("Button pressed!");
            pressed = true;
            invalidate();
            return true;
            
        case EventType::MouseRelease:
            LOGI("Button released!");
            if (pressed) {
                pressed = false;
                invalidate();
                if (clickListener) {
                    clickListener();
                }
                return true;
            }
            break;
            
        case EventType::MouseMove:
            return true;
            
        default:
            return TextView::onEvent(event);
    }
    return false;
}

void Button::onDraw(RenderContext& context) {
    // 绘制按钮背景
    Paint bgPaint;
    Color currentColor = pressed ? pressedColor : normalColor;
    bgPaint.setColor(currentColor);
    context.drawRect(bounds, bgPaint);
    
    // 调用父类绘制文本
    TextView::onDraw(context);
    LOGI("Button::onDraw - bounds(x=%d,y=%d,w=%d,h=%d) pressed=%d color=%d,%d,%d", 
         bounds.x, bounds.y, bounds.width, bounds.height, 
         pressed,
         currentColor.r, currentColor.g, currentColor.b);
} 