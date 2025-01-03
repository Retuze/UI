#include "graphics/render_context.h"
#include "core/logger.h"
#include "core/types.h"
#include "graphics/font_manager.h"
#include "graphics/surface.h"

LOG_TAG("RenderContext");

RenderContext::RenderContext() = default;

RenderContext::~RenderContext() = default;

bool RenderContext::initialize(int width, int height)
{
  surface = Surface::create(width, height);
  return surface && surface->initialize();
}

void RenderContext::clear(Color color)
{
  if (!surface)
    return;

  int stride;
  uint32_t pixelColor;
  auto* pixels = static_cast<uint32_t*>(surface->lock(&stride));
  if (!pixels)
    return;

  // 获取每个像素的字节数
  int bytesPerPixel;
  switch (surface->getPixelFormat()) {
  case PixelFormat::RGBA8888BE:
    pixelColor = color.toRGBA8888BE();
    bytesPerPixel = 4;
    break;
  case PixelFormat::BGRA8888BE:
    pixelColor = color.toBGRA8888BE();
    bytesPerPixel = 4;
    break;
  case PixelFormat::RGB888BE:
    pixelColor = color.toRGB888BE();
    bytesPerPixel = 3;
    break;
  case PixelFormat::RGB565BE:
    pixelColor = color.toRGB565BE();
    bytesPerPixel = 2;
    break;
  case PixelFormat::A8BE:
    pixelColor = color.toA8BE();
    bytesPerPixel = 1;
    break;
  case PixelFormat::RGBA8888LE:
    pixelColor = color.toRGBA8888LE();
    bytesPerPixel = 4;
    break;
  case PixelFormat::BGRA8888LE:
    pixelColor = color.toBGRA8888LE();
    bytesPerPixel = 4;
    break;
  case PixelFormat::RGB888LE:
    pixelColor = color.toRGB888LE();
    bytesPerPixel = 3;
    break;
  case PixelFormat::RGB565LE:
    pixelColor = color.toRGB565LE();
    bytesPerPixel = 2;
    break;
  case PixelFormat::A8LE:
    pixelColor = color.toA8LE();
    bytesPerPixel = 1;
    break;
  default:
    pixelColor = color.toRGBA8888BE();
    bytesPerPixel = 4;
    break;
  }

  // 计算实际的像素步长（以像素为单位，而不是字节）
  int pixelStride = stride / bytesPerPixel;

  for (int y = 0; y < surface->getHeight(); ++y) {
    uint32_t* row = pixels + y * pixelStride; // 使用像素步长而不是字节步长
    for (int x = 0; x < surface->getWidth(); ++x) {
      row[x] = pixelColor;
    }
  }

  surface->unlock();
}

void RenderContext::drawRect(const Rect& rect, const Paint& paint)
{
  if (!surface) {
    LOGE("Surface is null!");
    return;
  }

  // 应用裁剪
  Rect clipRect = rect;
  if (!clipStack.empty()) {
    clipRect = clipRect.intersect(clipStack.back());
    if (clipRect.isEmpty())
      return;
  }

  int stride;
  uint32_t pixelColor;
  auto* pixels = static_cast<uint32_t*>(surface->lock(&stride));
  if (!pixels)
    return;
  // 获取每个像素的字节数
  int bytesPerPixel;
  switch (surface->getPixelFormat()) {

  case PixelFormat::BGRA8888BE:
    pixelColor = paint.getColor().toBGRA8888BE();
    bytesPerPixel = 4;
    break;
  case PixelFormat::RGBA8888BE:
    pixelColor = paint.getColor().toRGBA8888BE();
    bytesPerPixel = 4;
    break;
  case PixelFormat::RGB888BE:
    pixelColor = paint.getColor().toRGB888BE();
    bytesPerPixel = 3;
    break;
  case PixelFormat::RGB565BE:
    pixelColor = paint.getColor().toRGB565BE();
    bytesPerPixel = 2;
    break;
  case PixelFormat::A8BE:
    pixelColor = paint.getColor().toA8BE();
    bytesPerPixel = 1;
    break;
  case PixelFormat::BGRA8888LE:
    pixelColor = paint.getColor().toBGRA8888LE();
    bytesPerPixel = 4;
    break;
  case PixelFormat::RGBA8888LE:
    pixelColor = paint.getColor().toRGBA8888LE();
    bytesPerPixel = 4;
    break;
  case PixelFormat::RGB888LE:
    pixelColor = paint.getColor().toRGB888LE();
    bytesPerPixel = 3;
    break;
  case PixelFormat::RGB565LE:
    pixelColor = paint.getColor().toRGB565LE();
    bytesPerPixel = 2;
    break;
  case PixelFormat::A8LE:
    pixelColor = paint.getColor().toA8LE();
    bytesPerPixel = 1;
    break;
  default:
    pixelColor = paint.getColor().toRGBA8888BE();
    bytesPerPixel = 4;
    break;
  }

  // 计算实际的像素步长（以像素为单位，而不是字节）
  int pixelStride = stride / bytesPerPixel;

  for (int y = clipRect.y; y < clipRect.y + clipRect.height; ++y) {
    uint32_t* row = pixels + y * pixelStride; // 使用像素步长而不是字节步长
    for (int x = clipRect.x; x < clipRect.x + clipRect.width; ++x) {
      row[x] = pixelColor;
    }
  }

  surface->unlock();
}

void RenderContext::drawText(const std::string& text, float x, float y,
                             const Paint& paint)
{
    if (!surface) {
        LOGE("RenderContext::drawText - No surface available!");
        return;
    }

    // 获取FontManager实例
    auto& fontManager = FontManager::getInstance();
    
    // 应用裁剪
    Rect clipRect;
    if (!clipStack.empty()) {
        clipRect = clipStack.back();
    } else {
        clipRect = Rect(0, 0, surface->getWidth(), surface->getHeight());
    }

    // 锁定表面
    int stride;
    auto* pixels = static_cast<uint32_t*>(surface->lock(&stride));
    if (!pixels) {
        LOGE("RenderContext::drawText - Failed to lock surface!");
        return;
    }

    // 渲染文本
    fontManager.renderText(surface.get(), text, x, y, paint.getColor(), clipRect);

    surface->unlock();
}

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

void RenderContext::present()
{
  if (surface) {
    surface->present();
  }
}

void RenderContext::drawRoundRect(const Rect& rect, float radius,
                                  const Paint& paint)
{
  // For now, just draw a regular rectangle
  drawRect(rect, paint);
  // TODO: Implement proper rounded corners
}