#pragma once
#include "graphics/render_context.h"

// 渲染命令基类
class RenderCommand {
public:
    virtual ~RenderCommand() = default;
    virtual void execute(RenderContext& context) = 0;
};

// 清屏命令
class ClearCommand : public RenderCommand {
public:
    explicit ClearCommand(Color color) : color(color) {}
    void execute(RenderContext& context) override;
    
private:
    Color color;
};

// 绘制矩形命令
class DrawRectCommand : public RenderCommand {
public:
    DrawRectCommand(const Rect& rect, const Paint& paint) 
        : rect(rect), paint(paint) {}
    void execute(RenderContext& context) override;
    
private:
    Rect rect;
    Paint paint;
};

// 绘制文本命令
class DrawTextCommand : public RenderCommand {
public:
    DrawTextCommand(const std::string& text, float x, float y, const Paint& paint)
        : text(text), x(x), y(y), paint(paint) {}
    void execute(RenderContext& context) override;
    
private:
    std::string text;
    float x, y;
    Paint paint;
}; 