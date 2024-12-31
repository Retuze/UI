#pragma once
#include "view/view.h"
#include "graphics/paint.h"
#include <string>

enum class TextAlignment {
    Left,
    Center,
    Right
};

class TextView : public View {
public:
    TextView();
    explicit TextView(const std::string& text);
    
    // 文本相关方法
    void setText(const std::string& text);
    const std::string& getText() const { return text; }
    
    // 样式设置
    void setTextSize(float size);
    void setTextColor(Color color);
    void setTextAlignment(TextAlignment alignment);
    
    // 重写基类方法
    void onDraw(RenderContext& context) override;
    void onMeasure(int widthMeasureSpec, int heightMeasureSpec) override;
    
protected:
    std::string text;
    Paint textPaint;
    TextAlignment textAlignment = TextAlignment::Left;
    float padding = 8.0f;  // 默认内边距
}; 