#pragma once
#include "core/types.h"
#include <memory>

class Renderer {
public:
    virtual ~Renderer() = default;
    
    // 基础像素操作
    virtual void setPixel(int x, int y, Color color) = 0;
    virtual Color getPixel(int x, int y) const = 0;
    
    // 内存块操作
    virtual void blitRaw(int x, int y, const void* data, int width, int height, 
                        int stride, PixelFormat format) = 0;
    virtual void copyRect(const Rect& src, const Rect& dst) = 0;
    
    // 基础绘图操作
    virtual void drawLine(int x1, int y1, int x2, int y2, Color color) = 0;
    virtual void fillRect(const Rect& rect, Color color) = 0;
    
    // 缓冲区操作
    virtual void clear(Color color) = 0;
    virtual void present(const Rect* dirtyRects = nullptr, int numRects = 0) = 0;
    
    // 剪切区域
    virtual void pushClipRect(const Rect& rect) = 0;
    virtual void popClipRect() = 0;
    
    // 工厂方法
    static std::unique_ptr<Renderer> create(int width = 800, int height = 600, 
                                          const char* title = "SimpleGUI");
    
    virtual bool processEvents() = 0;  // 返回 false 表示应该退出
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    
    // 添加以下方法
    virtual void setVSync(bool enabled) = 0;  // 垂直同步控制
    virtual void setFrameRate(int fps) = 0;   // 帧率控制
    virtual bool isHardwareAccelerated() const = 0;  // 是否硬件加速

protected:
    // 辅助函数
    virtual void* lockRect(const Rect& rect, int* stride) = 0;
    virtual void unlockRect() = 0;
};