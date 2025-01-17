#include "graphics/freetype_wrapper.h"

FreeTypeWrapper::FreeTypeWrapper() : library(nullptr) {}

FreeTypeWrapper::~FreeTypeWrapper() {
    cleanup();
}

bool FreeTypeWrapper::initialize() {
    return FT_Init_FreeType(&library) == 0;
}

void FreeTypeWrapper::cleanup() {
    if (library) {
        FT_Done_FreeType(library);
        library = nullptr;
    }
}

FT_Error FreeTypeWrapper::loadFace(const std::string& fontPath, FT_Face* face) {
    return FT_New_Face(library, fontPath.c_str(), 0, face);
}

void FreeTypeWrapper::destroyFace(FT_Face face) {
    if (face) {
        FT_Done_Face(face);
    }
}

bool FreeTypeWrapper::renderGlyph(FT_Face face, uint32_t glyphIndex, int size) {
    if (!face) return false;
    
    FT_Set_Pixel_Sizes(face, 0, size);
    
    if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT) != 0) {
        return false;
    }
    
    return FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) == 0;
}

void FreeTypeWrapper::drawGlyphBitmap(
    Bitmap* target,
    const FT_Bitmap& bitmap,
    int x, int y,
    Color color) {
    
    if (!target) return;
    
    const uint8_t* src = bitmap.buffer;
    for (unsigned int row = 0; row < bitmap.rows; row++) {
        for (unsigned int col = 0; col < bitmap.width; col++) {
            int px = x + col;
            int py = y + row;
            
            if (px < 0 || py < 0 || 
                px >= target->getWidth() || 
                py >= target->getHeight()) {
                continue;
            }
            
            uint8_t alpha = src[col];
            if (alpha > 0) {
                Color pixelColor = color;
                pixelColor.a = (color.a * alpha) / 255;
                target->setPixel(px, py, pixelColor);
            }
        }
        src += bitmap.pitch;
    }
}

IFontRenderer::GlyphMetrics FreeTypeWrapper::getGlyphMetrics(
    FT_Face face,
    uint32_t glyphIndex,
    int size) {
    
    IFontRenderer::GlyphMetrics metrics{};
    if (!face) return metrics;
    
    FT_Set_Pixel_Sizes(face, 0, size);
    if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT) != 0) {
        return metrics;
    }
    
    metrics.width = face->glyph->metrics.width >> 6;
    metrics.height = face->glyph->metrics.height >> 6;
    metrics.horiBearingX = face->glyph->metrics.horiBearingX >> 6;
    metrics.horiBearingY = face->glyph->metrics.horiBearingY >> 6;
    metrics.horiAdvance = face->glyph->metrics.horiAdvance >> 6;
    metrics.vertBearingX = face->glyph->metrics.vertBearingX >> 6;
    metrics.vertBearingY = face->glyph->metrics.vertBearingY >> 6;
    metrics.vertAdvance = face->glyph->metrics.vertAdvance >> 6;
    
    return metrics;
} 