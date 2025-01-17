#include "graphics/text_renderer.h"

TextRenderer::TextRenderer() {
    ftWrapper.initialize();
}

TextRenderer::~TextRenderer() {
    for (auto& [name, context] : fonts) {
        if (context->hb_font) {
            hbWrapper.destroyFont(context->hb_font);
        }
        if (context->ft_face) {
            ftWrapper.destroyFace(context->ft_face);
        }
    }
    ftWrapper.cleanup();
}

bool TextRenderer::loadFont(const std::string& fontPath, const std::string& name) {
    auto context = std::make_unique<FontContext>();
    context->path = fontPath;
    
    // 加载 FreeType 字体
    FT_Error error = ftWrapper.loadFace(fontPath, &context->ft_face);
    if (error) {
        return false;
    }
    
    // 创建 HarfBuzz 字体
    context->hb_font = hbWrapper.createFont(context->ft_face);
    if (!context->hb_font) {
        ftWrapper.destroyFace(context->ft_face);
        return false;
    }
    
    fonts[name] = std::move(context);
    return true;
}

std::vector<IFontRenderer::ShapedGlyph> TextRenderer::shapeText(
    const std::string& text,
    const TextStyle& style) {
    
    auto it = fonts.find(style.fontName);
    if (it == fonts.end()) {
        return {};
    }
    
    return hbWrapper.shapeText(it->second->hb_font, text, style);
}

void TextRenderer::renderShapedText(
    Bitmap* bitmap,
    const std::vector<ShapedGlyph>& shaped,
    const TextStyle& style,
    int x, int y) {
    
    auto it = fonts.find(style.fontName);
    if (it == fonts.end()) {
        return;
    }
    
    float pen_x = x;
    float pen_y = y;
    
    for (const auto& glyph : shaped) {
        if (ftWrapper.renderGlyph(it->second->ft_face, glyph.glyphId, style.size)) {
            int glyph_x = pen_x + glyph.x_offset;
            int glyph_y = pen_y - it->second->ft_face->glyph->bitmap_top + glyph.y_offset;
            
            ftWrapper.drawGlyphBitmap(bitmap,
                                    it->second->ft_face->glyph->bitmap,
                                    glyph_x,
                                    glyph_y,
                                    style.color);
        }
        pen_x += glyph.x_advance;
        pen_y += glyph.y_advance;
    }
}

void TextRenderer::renderText(
    Bitmap* bitmap,
    const std::string& text,
    const TextStyle& style,
    int x, int y) {
    
    auto shaped = shapeText(text, style);
    renderShapedText(bitmap, shaped, style, x, y);
}

Size TextRenderer::getTextSize(
    const std::string& text,
    const TextStyle& style) {
    
    auto shaped = shapeText(text, style);
    if (shaped.empty()) {
        return {0, 0};
    }
    
    float width = 0;
    float height = 0;
    
    auto it = fonts.find(style.fontName);
    if (it == fonts.end()) {
        return {0, 0};
    }
    
    for (const auto& glyph : shaped) {
        auto metrics = ftWrapper.getGlyphMetrics(
            it->second->ft_face, glyph.glyphId, style.size);
        width += glyph.x_advance;
        height = std::max(height, static_cast<float>(metrics.height));
    }
    
    return {static_cast<int>(width), static_cast<int>(height)};
} 