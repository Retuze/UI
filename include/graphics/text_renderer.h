#pragma once
#include "graphics/IFontRenderer.h"
#include "graphics/freetype_wrapper.h"
#include "graphics/harfbuzz_wrapper.h"
#include <unordered_map>

class TextRenderer : public IFontRenderer {
private:
    FreeTypeWrapper ftWrapper;
    HarfBuzzWrapper hbWrapper;
    
    struct FontContext {
        FT_Face ft_face = nullptr;
        hb_font_t* hb_font = nullptr;
        std::string path;
    };
    
    std::unordered_map<std::string, std::unique_ptr<FontContext>> fonts;

public:
    TextRenderer();
    ~TextRenderer();
    
    bool loadFont(const std::string& fontPath, 
                 const std::string& name = "default") override;
                 
    void renderText(Bitmap* bitmap,
                   const std::string& text,
                   const TextStyle& style,
                   int x, int y) override;
                   
    Size getTextSize(const std::string& text,
                    const TextStyle& style) override;

private:
    std::vector<ShapedGlyph> shapeText(
        const std::string& text,
        const TextStyle& style);
    void renderShapedText(
        Bitmap* bitmap,
        const std::vector<ShapedGlyph>& shaped,
        const TextStyle& style,
        int x, int y);
}; 