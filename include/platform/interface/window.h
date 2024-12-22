#pragma once
#include <cstdint>
#include <string>
#include "font/font.h"
#include <functional>

namespace ui {

// 基础类型定义
struct Size {
    int width;
    int height;
    
    Size(int w, int h) : width(w), height(h) {}
};

struct Point {
    int x;
    int y;
    
    Point(int x_, int y_) : x(x_), y(y_) {}
};

// 窗口接口
class IWindow {
public:
    virtual ~IWindow() = default;
    
    // 窗口操作
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void Close() = 0;
    virtual void Update() = 0;
    
    // 绘图操作
    virtual void SetPixel(int x, int y, uint8_t b, uint8_t g, uint8_t r, uint8_t a = 255) = 0;
    virtual void Clear(uint8_t b = 0, uint8_t g = 0, uint8_t r = 0, uint8_t a = 255) = 0;
    
    // 属性访问
    virtual Size GetSize() const = 0;
    virtual void SetSize(const Size& size) = 0;
    virtual Point GetPosition() const = 0;
    virtual void SetPosition(const Point& pos) = 0;
    virtual const char* GetTitle() const = 0;
    virtual void SetTitle(const char* title) = 0;
    
    // 消息循环
    virtual void RunMessageLoop() = 0;
    virtual bool IsRunning() const = 0;
    
    // 字体渲染 - 重命名方法避免冲突
    virtual FontPtr CreateFontObject() = 0;
    virtual void RenderText(const char* text, int x, int y, 
                          const Color& color, FontPtr font) = 0;
    
};

} // namespace ui 