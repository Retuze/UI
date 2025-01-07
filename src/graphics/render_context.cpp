// 1. 基础设施
#include "graphics/render_context.h"
#include "core/logger.h"
#include "core/types.h"
#include "graphics/font_manager.h"
#include "graphics/surface.h"
#include "graphics/pixel_writer.h"

LOG_TAG("RenderContext");

// 2. 构造和初始化
RenderContext::RenderContext() = default;

RenderContext::~RenderContext() = default;

bool RenderContext::initialize(int width, int height)
{
  surface = Surface::create(SurfaceConfig{width, height, PixelFormat::BGRA8888LE, 3, false});
  if (!surface || !surface->initialize()) {
    return false;
  }
  
  // 创建对应的 PixelWriter
  pixelWriter = PixelWriter::create(surface->getPixelFormat());
  return true;
}

// 3. 状态管理
void RenderContext::save()
{
  if (clipStack.empty()) {
    clipStack.push_back(Rect(0, 0, surface->getWidth(), surface->getHeight()));
  } else {
    clipStack.push_back(clipStack.back());
  }
}

void RenderContext::restore()
{
  if (!clipStack.empty()) {
    clipStack.pop_back();
  }
}

void RenderContext::clipRect(const Rect& rect)
{
  if (!clipStack.empty()) {
    Rect current = clipStack.back();
    clipStack.back() = current.intersect(rect);
  } else {
    clipStack.push_back(rect);
  }
}

// 4. 基础绘制操作
void RenderContext::clear(Color color)
{
  if (!surface || !pixelWriter)
    return;

  void* pixels = surface->dequeueBuffer();
  if (!pixels)
    return;

  // 使用 PixelWriter 填充像素
  auto* dst = static_cast<uint8_t*>(pixels);
  int width = surface->getWidth();
  int height = surface->getHeight();
  int bytesPerPixel = pixelWriter->getBytesPerPixel();
  
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      pixelWriter->writePixel(dst + (y * width + x) * bytesPerPixel, color);
    }
  }

  surface->queueBuffer();
}

void RenderContext::drawRect(const Rect& rect, const Paint& paint)
{
    if (!surface || !pixelWriter)
        return;

    // 应用裁剪
    Rect clipRect = rect;
    if (!clipStack.empty()) {
        clipRect = clipRect.intersect(clipStack.back());
        if (clipRect.isEmpty())
            return;
    }

    // 获取缓冲区
    void* pixels = surface->dequeueBuffer();
    if (!pixels)
        return;

    // 使用 PixelWriter 绘制矩形
    auto* dst = static_cast<uint8_t*>(pixels);
    int width = surface->getWidth();
    int bytesPerPixel = pixelWriter->getBytesPerPixel();
    
    for (int y = clipRect.y; y < clipRect.y + clipRect.height; ++y) {
        for (int x = clipRect.x; x < clipRect.x + clipRect.width; ++x) {
            pixelWriter->writePixel(dst + (y * width + x) * bytesPerPixel, 
                                  paint.getColor());
        }
    }

    surface->queueBuffer();
}

// 6. 文本渲染
void RenderContext::drawText(const std::string& text, float x, float y, const Paint& paint)
{
    if (!surface || !pixelWriter) {
        LOGE("RenderContext::drawText - No surface or pixel writer available!");
        return;
    }

    auto& fontManager = FontManager::getInstance();
    
    Rect clipRect;
    if (!clipStack.empty()) {
        clipRect = clipStack.back();
    } else {
        clipRect = Rect(0, 0, surface->getWidth(), surface->getHeight());
    }

    void* pixels = surface->dequeueBuffer();
    if (!pixels)
        return;

    // TODO: 渲染文本
    // fontManager.renderText(pixels, surface->getWidth(), surface->getHeight(),
    //                      pixelWriter.get(), text, x, y, paint.getColor(), clipRect);

    surface->queueBuffer();
}

void RenderContext::drawRoundRect(const Rect& rect, float radius, const Paint& paint)
{
    if (!surface || !pixelWriter)
        return;

    // 应用裁剪
    Rect clipRect = rect;
    if (!clipStack.empty()) {
        clipRect = clipRect.intersect(clipStack.back());
        if (clipRect.isEmpty())
            return;
    }

    // 获取缓冲区
    void* pixels = surface->dequeueBuffer();
    if (!pixels)
        return;

    auto* dst = static_cast<uint8_t*>(pixels);
    int width = surface->getWidth();
    int bytesPerPixel = pixelWriter->getBytesPerPixel();
    Color color = paint.getColor();

    int r = static_cast<int>(radius);
    for (int y = clipRect.y; y < clipRect.y + clipRect.height; ++y) {
        for (int x = clipRect.x; x < clipRect.x + clipRect.width; ++x) {
            // 检查点是否在圆角矩形内
            bool inside = true;
            
            // 左上角
            if (x < rect.x + r && y < rect.y + r) {
                inside = std::pow(x - (rect.x + r), 2) + 
                        std::pow(y - (rect.y + r), 2) <= r * r;
            }
            // 右上角
            else if (x >= rect.x + rect.width - r && y < rect.y + r) {
                inside = std::pow(x - (rect.x + rect.width - r), 2) + 
                        std::pow(y - (rect.y + r), 2) <= r * r;
            }
            // 左下角
            else if (x < rect.x + r && y >= rect.y + rect.height - r) {
                inside = std::pow(x - (rect.x + r), 2) + 
                        std::pow(y - (rect.y + rect.height - r), 2) <= r * r;
            }
            // 右下角
            else if (x >= rect.x + rect.width - r && y >= rect.y + rect.height - r) {
                inside = std::pow(x - (rect.x + rect.width - r), 2) + 
                        std::pow(y - (rect.y + rect.height - r), 2) <= r * r;
            }

            if (inside) {
                pixelWriter->writePixel(dst + (y * width + x) * bytesPerPixel, color);
            }
        }
    }

    surface->queueBuffer();
}

void RenderContext::drawLine(float x1, float y1, float x2, float y2, const Paint& paint)
{
    if (!surface || !pixelWriter)
        return;

    // 应用裁剪
    Rect clipRect;
    if (!clipStack.empty()) {
        clipRect = clipStack.back();
    } else {
        clipRect = Rect(0, 0, surface->getWidth(), surface->getHeight());
    }

    // 获取缓冲区
    void* pixels = surface->dequeueBuffer();
    if (!pixels)
        return;

    auto* dst = static_cast<uint8_t*>(pixels);
    int width = surface->getWidth();
    int bytesPerPixel = pixelWriter->getBytesPerPixel();
    Color color = paint.getColor();

    // Bresenham直线算法
    int dx = static_cast<int>(std::abs(x2 - x1));
    int dy = static_cast<int>(std::abs(y2 - y1));
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    int x = static_cast<int>(x1);
    int y = static_cast<int>(y1);

    while (true) {
        if (x >= clipRect.x && x < clipRect.x + clipRect.width &&
            y >= clipRect.y && y < clipRect.y + clipRect.height) {
            pixelWriter->writePixel(dst + (y * width + x) * bytesPerPixel, color);
        }

        if (x == static_cast<int>(x2) && y == static_cast<int>(y2))
            break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }

    surface->queueBuffer();
}

void RenderContext::drawCircle(float x, float y, float radius, const Paint& paint)
{
    if (!surface || !pixelWriter)
        return;

    // 应用裁剪
    Rect clipRect;
    if (!clipStack.empty()) {
        clipRect = clipStack.back();
    } else {
        clipRect = Rect(0, 0, surface->getWidth(), surface->getHeight());
    }

    // 获取缓冲区
    void* pixels = surface->dequeueBuffer();
    if (!pixels)
        return;

    auto* dst = static_cast<uint8_t*>(pixels);
    int width = surface->getWidth();
    int bytesPerPixel = pixelWriter->getBytesPerPixel();
    Color color = paint.getColor();

    // 中点圆算法
    int cx = static_cast<int>(x);
    int cy = static_cast<int>(y);
    int r = static_cast<int>(radius);
    int px = 0;
    int py = r;
    int d = 1 - r;

    auto drawCirclePoints = [&](int x, int y) {
        const int points[8][2] = {
            {cx + x, cy + y}, {cx - x, cy + y},
            {cx + x, cy - y}, {cx - x, cy - y},
            {cx + y, cy + x}, {cx - y, cy + x},
            {cx + y, cy - x}, {cx - y, cy - x}
        };

        for (const auto& p : points) {
            if (p[0] >= clipRect.x && p[0] < clipRect.x + clipRect.width &&
                p[1] >= clipRect.y && p[1] < clipRect.y + clipRect.height) {
                pixelWriter->writePixel(dst + (p[1] * width + p[0]) * bytesPerPixel, color);
            }
        }
    };

    while (px <= py) {
        drawCirclePoints(px, py);
        px++;
        if (d < 0) {
            d += 2 * px + 1;
        } else {
            py--;
            d += 2 * (px - py) + 1;
        }
    }

    surface->queueBuffer();
}

// 7. 缓冲区操作
void RenderContext::present()
{
    if (surface) {
        surface->present();
    }
}