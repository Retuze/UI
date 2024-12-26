#pragma once
#include "renderer.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <memory>
#include <unordered_map>
#include "core/types.h"

class FontManager {
public:
    static FontManager& getInstance();
    
    bool initialize();
    bool loadFont(const std::string& fontPath, int size);
    void renderText(Renderer* renderer, const std::string& text, int x, int y, Color color);

private:
    FontManager() = default;
    ~FontManager();
    
    FT_Library library = nullptr;
    FT_Face face = nullptr;
}; 