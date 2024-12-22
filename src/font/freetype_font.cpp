#include "font/freetype_font.h"
#include "platform/interface/window.h"
#include <stdexcept>
#include <windows.h>

namespace ui {

FreetypeFont::FreetypeFont(IWindow* window)
    : m_ft(nullptr)
    , m_face(nullptr)
    , m_window(window)
    , m_size(0)
{
    if (FT_Init_FreeType(&m_ft)) {
        throw std::runtime_error("Failed to initialize FreeType");
    }
}

FreetypeFont::~FreetypeFont() {
    if (m_face) {
        FT_Done_Face(m_face);
    }
    if (m_ft) {
        FT_Done_FreeType(m_ft);
    }
}

bool FreetypeFont::LoadFromFile(const char* path, float size) {
    // Convert UTF-8 path to wide string for Windows API
    std::wstring widePath = Utf8ToWide(path);
    
    m_size = static_cast<int>(size);
    
    if (FT_Init_FreeType(&m_ft)) {
        return false;
    }

    if (FT_New_Face(m_ft, WideToUtf8(widePath).c_str(), 0, &m_face)) {
        FT_Done_FreeType(m_ft);
        return false;
    }

    FT_Set_Pixel_Sizes(m_face, 0, m_size);
    return true;
}

void FreetypeFont::RenderText(const char* text, int x, int y, const Color& color) {
    if (!text || !m_face) return;
    
    // Convert UTF-8 text to wide string for proper Unicode handling
    std::wstring wideText = Utf8ToWide(text);
    
    FT_GlyphSlot slot = m_face->glyph;
    int pen_x = x;
    
    for (wchar_t ch : wideText) {
        if (FT_Load_Char(m_face, ch, FT_LOAD_RENDER)) {
            continue;
        }

        // Render the glyph bitmap
        FT_Bitmap* bitmap = &slot->bitmap;
        for (unsigned int row = 0; row < bitmap->rows; row++) {
            for (unsigned int col = 0; col < bitmap->width; col++) {
                unsigned char value = bitmap->buffer[row * bitmap->pitch + col];
                if (value > 0) {
                    int draw_x = pen_x + slot->bitmap_left + col;
                    int draw_y = y - slot->bitmap_top + row;
                    
                    // Blend the pixel with alpha
                    uint8_t alpha = (value * color.a) / 255;
                    m_window->SetPixel(draw_x, draw_y, 
                        color.b, color.g, color.r, alpha);
                }
            }
        }
        
        pen_x += slot->advance.x >> 6;
    }
}

int FreetypeFont::GetTextWidth(const char* text) const {
    if (!text || !m_face) return 0;
    
    std::wstring wideText = Utf8ToWide(text);
    int width = 0;
    
    for (wchar_t ch : wideText) {
        if (FT_Load_Char(m_face, ch, FT_LOAD_DEFAULT)) {
            continue;
        }
        width += m_face->glyph->advance.x >> 6;
    }
    
    return width;
}

int FreetypeFont::GetTextHeight() const {
    return m_size;
}

// Helper functions for UTF-8 conversion
std::wstring FreetypeFont::Utf8ToWide(const char* utf8Str) {
    if (!utf8Str) return std::wstring();
    
    int wideLength = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, nullptr, 0);
    if (wideLength == 0) return std::wstring();
    
    std::wstring wideStr(wideLength - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, &wideStr[0], wideLength);
    return wideStr;
}

std::string FreetypeFont::WideToUtf8(const std::wstring& wideStr) {
    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, 
                                       nullptr, 0, nullptr, nullptr);
    if (utf8Length == 0) return std::string();
    
    std::string utf8Str(utf8Length - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, 
                       &utf8Str[0], utf8Length, nullptr, nullptr);
    return utf8Str;
}

} // namespace ui 