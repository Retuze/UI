// 1. 基础设施
#include "graphics/render_context.h"
#include "core/logger.h"
#include "core/types.h"
#include "graphics/surface.h"
#include "graphics/pixel.h"
#include "graphics/text_renderer.h"
#include "graphics/IFontRenderer.h"

LOG_TAG("RenderContext");

// 2. 构造和初始化
RenderContext::RenderContext() {
    fontRenderer = createDefaultFontRenderer();
}

RenderContext::~RenderContext() = default;

void RenderContext::beginFrame(Surface* surface) {
    if (!surface) {
        LOGE("beginFrame with null surface");
        return;
    }
    
    currentSurface = surface;
    // 获取绘制缓冲区
    currentBitmap = currentSurface->lockBuffer();
    if (!currentBitmap) {
        LOGE("Failed to lock buffer");
        currentSurface = nullptr;
        return;
    }
    
    // 初始化默认状态
    currentState = State{};
    currentState.clipRect = Rect(0, 0, surface->getWidth(), surface->getHeight());
    stateStack = std::stack<State>();
}

void RenderContext::endFrame() {
    if (!currentSurface) {
        return;
    }
    
    currentSurface->unlockBuffer();
    currentSurface->present();
    currentSurface = nullptr;
    currentBitmap = nullptr;
}

// 3. 状态管理
void RenderContext::save() {
    stateStack.push(currentState);
}

void RenderContext::restore() {
    if (!stateStack.empty()) {
        currentState = stateStack.top();
        stateStack.pop();
    }
}

void RenderContext::clipRect(const Rect& rect) {
    currentState.clipRect = currentState.clipRect.intersect(rect);
}

bool RenderContext::checkSurface() const {
    if (!currentSurface) {
        LOGE("No surface available for drawing");
        return false;
    }
    return true;
}

// 变换操作
void RenderContext::translate(float dx, float dy) {
    currentState.transform = currentState.transform * Matrix::makeTranslate(dx, dy);
}

void RenderContext::rotate(float degrees) {
    currentState.transform = currentState.transform * Matrix::makeRotate(degrees);
}

void RenderContext::scale(float sx, float sy) {
    currentState.transform = currentState.transform * Matrix::makeScale(sx, sy);
}

void RenderContext::setMatrix(const Matrix& matrix) {
    currentState.transform = matrix;
}

// 绘制状态控制
void RenderContext::setAlpha(float alpha) {
    currentState.alpha = alpha;
}

void RenderContext::setBlendMode(BlendMode mode) {
    currentState.blendMode = mode;
}

void RenderContext::setAntiAlias(bool enabled) {
    currentState.antiAlias = enabled;
}

// 基础绘制操作
void RenderContext::clear(Color color) {
    if (!checkSurface()) return;
    
    int width = currentBitmap->getWidth();
    int height = currentBitmap->getHeight();
    
    // 遍历每个像素进行设置
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            currentBitmap->setPixel(x, y, color);
        }
    }
}

void RenderContext::drawRect(const Rect& rect, const Paint& paint) {
    if (!checkSurface()) return;
    
    // 应用变换
    Rect transformedRect = currentState.transform.mapRect(rect);
    
    // 应用裁剪
    Rect clipRect = transformedRect.intersect(currentState.clipRect);
    if (clipRect.isEmpty()) return;
    
    Color color = paint.getColor();
    color.a = static_cast<uint8_t>(color.a * currentState.alpha);
    
    // 遍历裁剪区域内的像素
    for (int y = clipRect.y; y < clipRect.y + clipRect.height; ++y) {
        for (int x = clipRect.x; x < clipRect.x + clipRect.width; ++x) {
            if (currentState.blendMode == BlendMode::SrcOver) {
                // 读取目标像素
                Color dstColor = currentBitmap->getPixel(x, y);
                // 混合颜色
                Color blended = blendColors(color, dstColor, currentState.blendMode);
                currentBitmap->setPixel(x, y, blended);
            } else {
                currentBitmap->setPixel(x, y, color);
            }
        }
    }
}

void RenderContext::fillRect(const Rect& rect, const Color& color) {
    if (!checkSurface()) return;
    
    // 应用变换
    Rect transformedRect = currentState.transform.mapRect(rect);
    
    // 应用裁剪
    Rect clipRect = transformedRect.intersect(currentState.clipRect);
    if (clipRect.isEmpty()) return;
    
    // 调整alpha
    Color finalColor = color;
    finalColor.a = static_cast<uint8_t>(color.a * currentState.alpha);
    
    // 按行批量填充
    for (int y = clipRect.y; y < clipRect.y + clipRect.height; ++y) {
        if (currentState.blendMode == BlendMode::SrcOver) {
            blendHLine(clipRect.x, y, clipRect.width, finalColor);
        } else {
            fillHLine(clipRect.x, y, clipRect.width, finalColor);
        }
    }
}

void RenderContext::fillHLine(int x, int y, int width, const Color& color) {
    // 使用 memset 或 std::fill 进行快速填充
    if (currentBitmap->getFormat().baseFormat == BasePixelFormat::RGBA8888 ||
        currentBitmap->getFormat().baseFormat == BasePixelFormat::BGRA8888) {
        // 获取行起始位置
        uint32_t* row = reinterpret_cast<uint32_t*>(
            currentBitmap->getPixels() + 
            y * currentBitmap->getStride() + 
            x * currentBitmap->getBytesPerPixel()
        );
        
        // 准备颜色值
        uint32_t packed = (currentBitmap->getFormat().baseFormat == BasePixelFormat::RGBA8888) 
            ? color.toRGBA8888() 
            : color.toBGRA8888();
            
        // 快速填充
        std::fill_n(row, width, packed);
    } else {
        // 对于其他格式，逐像素设置
        for (int i = 0; i < width; ++i) {
            currentBitmap->setPixel(x + i, y, color);
        }
    }
}

void RenderContext::blendHLine(int x, int y, int width, const Color& src) {
    // 对于需要混合的情况，我们可以使用SIMD优化
    // 这里展示基础实现
    for (int i = 0; i < width; ++i) {
        Color dst = currentBitmap->getPixel(x + i, y);
        Color blended = blendColors(src, dst, currentState.blendMode);
        currentBitmap->setPixel(x + i, y, blended);
    }
}

void RenderContext::drawText(const std::string& text, float x, float y, const Paint& paint) {
    if (!checkSurface() || !currentBitmap) {
        return;
    }

    // 创建文本样式
    TextStyle style;
    style.size = static_cast<int>(paint.getTextSize());
    style.color = paint.getColor();
    style.color.a = static_cast<uint8_t>(style.color.a * currentState.alpha);

    // 应用当前变换
    Point transformed = currentState.transform.mapPoint(Point(x, y));

    // 渲染文本
    fontRenderer->renderText(
        currentBitmap,
        text,
        style,
        static_cast<int>(transformed.x),
        static_cast<int>(transformed.y)
    );
}