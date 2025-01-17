#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "graphics/bitmap.h"
#include "graphics/IFontRenderer.h"

class FreeTypeWrapper {
public:
    FreeTypeWrapper();
    ~FreeTypeWrapper();
    
    bool initialize();
    void cleanup();
    
    FT_Error loadFace(const std::string& fontPath, FT_Face* face);
    void destroyFace(FT_Face face);
    
    bool renderGlyph(FT_Face face, uint32_t glyphIndex, int size);
    void drawGlyphBitmap(Bitmap* target, const FT_Bitmap& bitmap,
                        int x, int y, Color color);
    IFontRenderer::GlyphMetrics getGlyphMetrics(FT_Face face, 
                                               uint32_t glyphIndex,
                                               int size);
private:
    FT_Library library = nullptr;
}; 