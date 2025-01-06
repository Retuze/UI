#include "widgets/text_view.h"
#include "view/measure_spec.h"
#include "core/logger.h"

LOG_TAG("TextView");

TextView::TextView() : TextView("") {}

TextView::TextView(const std::string& text) : text(text) {
    textPaint.setTextSize(16.0f);  // 默认字体大小
    textPaint.setColor(Color::Black());  // 默认颜色
}

void TextView::setText(const std::string& text) {
    if (this->text != text) {
        this->text = text;
        requestLayout();
        invalidate();
    }
}

void TextView::setTextSize(float size) {
    textPaint.setTextSize(size);
    requestLayout();
    invalidate();
}

void TextView::setTextColor(Color color) {
    textPaint.setColor(color);
    invalidate();
}

void TextView::setTextAlignment(TextAlignment alignment) {
    textAlignment = alignment;
    invalidate();
}

void TextView::onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
    // 计算文本尺寸
    float textWidth = textPaint.measureText(text);
    float textHeight = textPaint.getTextHeight();
    
    // 文本实际占用的高度约为 1.2 倍的字体高度
    float actualTextHeight = textHeight * 1.2f;
    
    // 考虑padding
    int desiredWidth = static_cast<int>(textWidth + paddingLeft + paddingRight);
    int desiredHeight = static_cast<int>(actualTextHeight + paddingTop + paddingBottom);
    
    // 根据MeasureSpec调整最终尺寸
    int width = MeasureSpec::resolveSize(desiredWidth, widthMeasureSpec);
    int height = MeasureSpec::resolveSize(desiredHeight, heightMeasureSpec);
    
    setMeasuredDimension(width, height);
}

void TextView::onDraw(RenderContext& context) {
    // 计算文本位置
    float textWidth = textPaint.measureText(text);
    float textHeight = textPaint.getTextHeight();
    
    // 计算文本垂直居中的基线位置
    // 文本基线应该在中心偏下textHeight/2的位置
    float textBaseline = bounds.y + (bounds.height + textHeight) / 2.0f - textHeight * 0.1f;
    
    // 根据对齐方式调整x坐标
    float x = bounds.x + paddingLeft;
    if (textAlignment == TextAlignment::Center) {
        x = bounds.x + (bounds.width - textWidth) / 2;
    }
    
    // 绘制文本
    context.drawText(text, x, textBaseline, textPaint);
} 