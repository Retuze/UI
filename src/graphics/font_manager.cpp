#include "graphics/font_manager.h"
#include "freetype/ftlcdfil.h"
#include <stdexcept>
#include <windows.h>
#include <algorithm>
#include <cmath>
#include "graphics/dpi_manager.h"
#include "core/logger.h"

LOG_TAG("FontManager");

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
        LOGE("Failed to load font: %s", fontPath.c_str());
        return false;
    }
    
    auto& dpiManager = DPIManager::getInstance();
    int adjustedSize = dpiManager.scaleY(size);
    
    LOGD("Adjusted font size: %d (DPI: %d)", adjustedSize, dpiManager.getDpiY());
    
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    FT_Set_Pixel_Sizes(face, 0, adjustedSize);
    return true;
}

void FontManager::renderText(Surface* surface, const std::string& text, 
                           float x, float y, Color color, const Rect& clipRect) {
    
    if (!face) {
        LOGE("FontManager::renderText - No font face loaded!");
        return;
    }
    if (!surface) {
        LOGE("FontManager::renderText - No surface provided!");
        return;
    }
    
    int pen_x = static_cast<int>(x * 64);  // 26.6 固定小数点格式
    int pen_y = static_cast<int>(y * 64);
    
    int stride;
    auto* pixels = static_cast<uint32_t*>(surface->lock(&stride));
    if (!pixels) {
        LOGE("FontManager::renderText - Failed to lock surface!");
        return;
    }
    
    FT_Int32 loadFlags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;
    
    // 遍历并渲染每个字符
    const unsigned char* p = reinterpret_cast<const unsigned char*>(text.c_str());
    while (*p) {
        // UTF-8解码
        uint32_t charCode;
        int charLen = decodeUTF8(p, &charCode);
        if (charLen == 0) {
            LOGE("FontManager::renderText - Failed to decode UTF-8 character!");
            break;
        }
        
        p += charLen;
        
        // 加载字形
        if (FT_Load_Char(face, charCode, loadFlags)) {
            LOGE("FontManager::renderText - Failed to load glyph for character code: %u", charCode);
            continue;
        }
        
        FT_GlyphSlot slot = face->glyph;
        FT_Bitmap& bitmap = slot->bitmap;
        
        // 计算字形位置
        int x0 = pen_x + slot->bitmap_left * 64;
        int y0 = pen_y - slot->bitmap_top * 64;
        
        // 渲染字形
        renderGlyph(pixels, stride, bitmap, x0 >> 6, y0 >> 6, color, clipRect);
        
        // 更新笔位置
        pen_x += slot->advance.x;
        pen_y += slot->advance.y;
    }
    
    surface->unlock();
}

void FontManager::renderGlyph(uint32_t* pixels, int stride,
                            const FT_Bitmap& bitmap, int x, int y,
                            Color color, const Rect& clipRect) {
    // 遍历字形的每个像素
    for (unsigned int row = 0; row < bitmap.rows; row++) {
        int dy = y + row;
        // 跳过裁剪区域外的像素
        if (dy < clipRect.y || dy >= clipRect.y + clipRect.height) continue;
        
        for (unsigned int col = 0; col < bitmap.width; col++) {
            int dx = x + col;
            // 跳过裁剪区域外的像素
            if (dx < clipRect.x || dx >= clipRect.x + clipRect.width) continue;
            
            // 获取字形像素的alpha值
            unsigned char alpha = bitmap.buffer[row * bitmap.pitch + col];
            if (alpha == 0) continue;  // 跳过完全透明的像素
            
            // 计算目标像素位置
            uint32_t* pixel = pixels + dy * (stride / 4) + dx;
            
            // 读取背景色
            uint32_t bg = *pixel;
            uint8_t bg_r = (bg >> 16) & 0xFF;
            uint8_t bg_g = (bg >> 8) & 0xFF;
            uint8_t bg_b = bg & 0xFF;
            
            // Alpha混合
            float a = alpha / 255.0f;
            uint8_t r = static_cast<uint8_t>(color.r * a + bg_r * (1 - a));
            uint8_t g = static_cast<uint8_t>(color.g * a + bg_g * (1 - a));
            uint8_t b = static_cast<uint8_t>(color.b * a + bg_b * (1 - a));
            
            // 写回混合后的颜色
            *pixel = (r << 16) | (g << 8) | b;
        }
    }
}

int FontManager::decodeUTF8(const unsigned char* str, uint32_t* out_codepoint) {
    if ((*str & 0x80) == 0) {
        *out_codepoint = *str;
        return 1;
    } else if ((*str & 0xE0) == 0xC0) {
        if ((str[1] & 0xC0) != 0x80) return 0;
        *out_codepoint = ((str[0] & 0x1F) << 6) | (str[1] & 0x3F);
        return 2;
    } else if ((*str & 0xF0) == 0xE0) {
        if ((str[1] & 0xC0) != 0x80 || (str[2] & 0xC0) != 0x80) return 0;
        *out_codepoint = ((str[0] & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
        return 3;
    } else if ((*str & 0xF8) == 0xF0) {
        if ((str[1] & 0xC0) != 0x80 || (str[2] & 0xC0) != 0x80 || (str[3] & 0xC0) != 0x80) return 0;
        *out_codepoint = ((str[0] & 0x07) << 18) | ((str[1] & 0x3F) << 12) | 
                        ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
        return 4;
    }
    return 0;
}

FontManager::~FontManager() {
    if (face) {
        FT_Done_Face(face);
    }
    if (library) {
        FT_Done_FreeType(library);
    }
} 