#include "graphics/font_manager.h"
#include "freetype/ftlcdfil.h"
#include <stdexcept>
#include <windows.h>
#include <algorithm>
#include <cmath>

FontManager& FontManager::getInstance() {
    static FontManager instance;
    return instance;
}

bool FontManager::initialize() {
    if (FT_Init_FreeType(&library)) {
        return false;
    }
    return true;
}

bool FontManager::loadFont(const std::string& fontPath, int size) {
    if (face) {
        FT_Done_Face(face);
    }
    
    if (FT_New_Face(library, fontPath.c_str(), 0, &face)) {
        return false;
    }
    
    // 获取系统 DPI
    HDC hdc = GetDC(NULL);
    int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
    
    // 根据 DPI 调整字体大小（使用 72 DPI 作为基准）
    int adjustedSize = (size * dpi) / 72;
    
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    FT_Set_Pixel_Sizes(face, 0, adjustedSize);
    return true;
}

void FontManager::renderText(Renderer* renderer, const std::string& text, int x, int y, Color color) {
    if (!face) return;
    
    int pen_x = x;
    int pen_y = y;
    
    const unsigned char* str = (const unsigned char*)text.c_str();
    while (*str) {
        // UTF-8 解码部分保持不变
        uint32_t code;
        if ((*str & 0x80) == 0) {
            code = *str++;
        } else if ((*str & 0xE0) == 0xC0) {
            code = (*str++ & 0x1F) << 6;
            code |= (*str++ & 0x3F);
        } else if ((*str & 0xF0) == 0xE0) {
            code = (*str++ & 0x0F) << 12;
            code |= (*str++ & 0x3F) << 6;
            code |= (*str++ & 0x3F);
        } else {
            str++;
            continue;
        }
        
        // 使用普通渲染模式，不使用LCD
        if (FT_Load_Char(face, code, FT_LOAD_RENDER)) {
            continue;
        }
        
        FT_Bitmap& bitmap = face->glyph->bitmap;
        int left = pen_x + face->glyph->bitmap_left;
        int top = pen_y - face->glyph->bitmap_top;
        
        // 简化的alpha混合
        for (unsigned int row = 0; row < bitmap.rows; row++) {
            for (unsigned int col = 0; col < bitmap.width; col++) {
                unsigned char alpha = bitmap.buffer[row * bitmap.pitch + col];
                if (alpha > 0) {
                    float normalizedAlpha = alpha / 255.0f;
                    
                    Color pixelColor = color;
                    Color bgColor = renderer->getPixel(left + col, top + row);
                    
                    // 线性插值混合
                    pixelColor.r = (uint8_t)(color.r * normalizedAlpha + bgColor.r * (1 - normalizedAlpha));
                    pixelColor.g = (uint8_t)(color.g * normalizedAlpha + bgColor.g * (1 - normalizedAlpha));
                    pixelColor.b = (uint8_t)(color.b * normalizedAlpha + bgColor.b * (1 - normalizedAlpha));
                    
                    renderer->setPixel(left + col, top + row, pixelColor);
                }
            }
        }
        
        pen_x += face->glyph->advance.x >> 6;
    }
}

FontManager::~FontManager() {
    if (face) {
        FT_Done_Face(face);
    }
    if (library) {
        FT_Done_FreeType(library);
    }
} 