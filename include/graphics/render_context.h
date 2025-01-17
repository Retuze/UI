#pragma once
#include "core/types.h"
#include "graphics/blend_mode.h"
#include "graphics/surface.h"
#include "graphics/paint.h"
#include "graphics/matrix.h"
#include "graphics/path.h"
#include <stack>

// 前向声明
class Surface;
class PixelWriter;

class RenderContext {
public:
    // 构造和初始化
    RenderContext();
    ~RenderContext();
    
    // 绘制上下文设置
    void beginFrame(Surface* surface);  // 开始一帧绘制
    void endFrame();                    // 结束一帧绘制
    
    // 基础形状绘制
    void clear(Color color = Color::White());
    void drawLine(float x1, float y1, float x2, float y2, const Paint& paint);
    void drawRect(const Rect& rect, const Paint& paint);
    void drawCircle(float x, float y, float radius, const Paint& paint);
    void drawRoundRect(const Rect& rect, float radius, const Paint& paint);
    void drawPath(const Path& path, const Paint& paint);
    
    // 文本和图像
    void drawText(const std::string& text, float x, float y, const Paint& paint);
    void drawBitmap(const Bitmap& bitmap, float x, float y, const Paint& paint);
    
    // 状态管理
    void save();
    void restore();
    void clipRect(const Rect& rect);
    
    // 变换操作
    void translate(float dx, float dy);
    void rotate(float degrees);
    void scale(float sx, float sy);
    void setMatrix(const Matrix& matrix);
    
    // 绘制状态控制
    void setAlpha(float alpha);
    void setBlendMode(BlendMode mode);
    void setAntiAlias(bool enabled);
    
    // 批量像素操作
    void fillRect(const Rect& rect, const Color& color);
    
private:
    // 当前绘制状态
    struct State {
        Matrix transform;
        Rect clipRect;
        float alpha = 1.0f;
        BlendMode blendMode = BlendMode::SrcOver;
        bool antiAlias = true;
    };
    
    Surface* currentSurface = nullptr;
    Bitmap* currentBitmap = nullptr;
    std::stack<State> stateStack;
    State currentState;
    std::unique_ptr<PixelWriter> pixelWriter;
    
    // 辅助方法
    void applyState(const State& state);
    bool checkSurface() const;
    void fillHLine(int x, int y, int width, const Color& color);
    void blendHLine(int x, int y, int width, const Color& color);
}; 