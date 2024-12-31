#include "graphics/render_context.h"
#include "graphics/surface.h"
#include "graphics/font_manager.h"

RenderContext::RenderContext() = default;

RenderContext::~RenderContext() = default;

bool RenderContext::initialize(int width, int height) {
    surface = Surface::create(width, height);
    return surface && surface->initialize();
}

void RenderContext::clear(Color color) {
    if (!surface) return;
    
    int stride;
    auto* pixels = static_cast<uint32_t*>(surface->lock(&stride));
    if (!pixels) return;
    
    for (int y = 0; y < surface->getHeight(); ++y) {
        uint32_t* row = pixels + y * (stride / 4);
        for (int x = 0; x < surface->getWidth(); ++x) {
            row[x] = color.toARGB();
        }
    }
    
    surface->unlock();
}

void RenderContext::drawRect(const Rect& rect, const Paint& paint) {
    if (!surface) return;
    
    // 应用裁剪
    Rect clipRect = rect;
    if (!clipStack.empty()) {
        clipRect = clipRect.intersect(clipStack.back());
        if (clipRect.isEmpty()) return;
    }
    
    int stride;
    auto* pixels = static_cast<uint32_t*>(surface->lock(&stride));
    if (!pixels) return;
    
    // 填充矩形
    uint32_t color = paint.getColor().toARGB();
    for (int y = clipRect.y; y < clipRect.y + clipRect.height; ++y) {
        uint32_t* row = pixels + y * (stride / 4);
        for (int x = clipRect.x; x < clipRect.x + clipRect.width; ++x) {
            row[x] = color;
        }
    }
    
    surface->unlock();
}

void RenderContext::drawText(const std::string& text, float x, float y, const Paint& paint) {
    if (!surface) return;
    
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
    if (!pixels) return;
    
    // 渲染文本
    fontManager.renderText(surface.get(), text, x, y, paint.getColor(), clipRect);
    
    surface->unlock();
}

void RenderContext::save() {
    if (clipStack.empty()) {
        clipStack.push_back(Rect(0, 0, surface->getWidth(), surface->getHeight()));
    } else {
        clipStack.push_back(clipStack.back());
    }
}

void RenderContext::restore() {
    if (!clipStack.empty()) {
        clipStack.pop_back();
    }
}

void RenderContext::clipRect(const Rect& rect) {
    if (!clipStack.empty()) {
        Rect current = clipStack.back();
        clipStack.back() = current.intersect(rect);
    } else {
        clipStack.push_back(rect);
    }
}

void RenderContext::present() {
    if (surface) {
        surface->present();
    }
}

void RenderContext::drawRoundRect(const Rect& rect, float radius, const Paint& paint) {
    // For now, just draw a regular rectangle
    drawRect(rect, paint);
    // TODO: Implement proper rounded corners
} 