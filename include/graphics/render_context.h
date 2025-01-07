#pragma once
#include "core/types.h"
#include "graphics/paint.h"
#include "graphics/surface.h"
#include "graphics/pixel_writer.h"
#include <memory>
#include <vector>

class RenderContext {
public:
    // 构造和初始化
    RenderContext();
    ~RenderContext();
    bool initialize(int width = 800, int height = 600);
    
    // 基础形状绘制
    void clear(Color color = Color::White());
    void drawLine(float x1, float y1, float x2, float y2, const Paint& paint);
    void drawRect(const Rect& rect, const Paint& paint);
    void drawCircle(float x, float y, float radius, const Paint& paint);
    void drawRoundRect(const Rect& rect, float radius, const Paint& paint);
    
    // 文本和图像
    void drawText(const std::string& text, float x, float y, const Paint& paint);
    // TODO: 添加图像绘制
    // void drawBitmap(const Bitmap& bitmap, float x, float y, const Paint& paint);
    
    // 状态管理
    void save();
    void restore();
    void clipRect(const Rect& rect);
    
    // 缓冲区操作
    void present();
    
    // 属性访问
    Surface* getSurface() const { return surface.get(); }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
private:
    std::unique_ptr<Surface> surface;
    std::unique_ptr<PixelWriter> pixelWriter;
    std::vector<Rect> clipStack;
    int width = 0;
    int height = 0;
}; 