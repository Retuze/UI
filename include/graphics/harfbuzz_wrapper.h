#pragma once
#include <hb.h>
#include <hb-ft.h>
#include "graphics/IFontRenderer.h"

class HarfBuzzWrapper {
public:
    HarfBuzzWrapper() = default;
    ~HarfBuzzWrapper() = default;

    hb_font_t* createFont(FT_Face ftFace);
    void destroyFont(hb_font_t* font);
    
    std::vector<IFontRenderer::ShapedGlyph> shapeText(
        hb_font_t* font,
        const std::string& text,
        const TextStyle& style);
};