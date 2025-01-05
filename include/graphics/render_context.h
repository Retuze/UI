#pragma once
#include "core/types.h"
#include "graphics/paint.h"
#include "graphics/surface.h"
#include <memory>
#include <vector>

class RenderContext {
public:
    RenderContext();
    ~RenderContext();
    
    bool initialize(int width = 800, int height = 600);
    
    // 基础绘图操作
    void clear(Color color = Color::White());
    void drawRect(const Rect& rect, const Paint& paint);
    void drawText(const std::string& text, float x, float y, const Paint& paint);
    void drawBitmap(const Bitmap& bitmap, float x, float y, const Paint& paint);
    void drawRoundRect(const Rect& rect, float radius, const Paint& paint);
    
    // 状态管理
    void save();
    void restore();
    void clipRect(const Rect& rect);
    
    // 缓冲区操作
    void* lockPixels(int* stride);
    void unlockPixels();
    void present();
    
    Surface* getSurface() const { return surface.get(); }
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
private:
    std::unique_ptr<Surface> surface;
    std::vector<Rect> clipStack;
    int width = 0;
    int height = 0;
}; 