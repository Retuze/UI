#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "core/types.h"
#include "graphics/surface.h"
#include <string>

class FontManager {
public:
    static FontManager& getInstance();
    
    bool initialize();
    bool loadFont(const std::string& fontPath, int size = 16);
    void renderText(Surface* surface, const std::string& text, 
                   float x, float y, Color color, const Rect& clipRect);
    
private:
    FontManager() = default;
    ~FontManager();
    
    void renderGlyph(uint32_t* pixels, int stride, 
                    const FT_Bitmap& bitmap, int x, int y,
                    Color color, const Rect& clipRect);
    int decodeUTF8(const unsigned char* str, uint32_t* out_codepoint);
    
    FT_Library library = nullptr;
    FT_Face face = nullptr;
}; 