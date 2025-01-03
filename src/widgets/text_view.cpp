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
    float textWidth = textPaint.measureText(text);
    float textHeight = textPaint.getTextHeight();
    
    int width = static_cast<int>(textWidth + 2 * padding);
    int height = static_cast<int>(textHeight + 2 * padding);
    
    width = MeasureSpec::resolveSize(width, widthMeasureSpec);
    height = MeasureSpec::resolveSize(height, heightMeasureSpec);
    
    setMeasuredDimension(width, height);
}

void TextView::onDraw(RenderContext& context) {

    // 绘制背景
    Paint bgPaint;
    bgPaint.setColor(Color::White());
    context.drawRect(bounds, bgPaint);
    
    // 计算文本位置
    float textWidth = textPaint.measureText(text);
    float textHeight = textPaint.getTextHeight();
    
    // 修改基线位置计算
    float baseline = bounds.y + (bounds.height + textHeight) / 2;
    float x = bounds.x + (bounds.width - textWidth) / 2;
         
    context.drawText(text, x, baseline, textPaint);
} 