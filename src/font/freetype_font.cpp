#include "font/freetype_font.h"
#include "platform/interface/window.h"
#include <stdexcept>

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

bool FreetypeFont::LoadFromFile(const std::wstring& path, float size) {
    if (m_face) {
        FT_Done_Face(m_face);
        m_face = nullptr;
    }

    if (FT_New_Face(m_ft, std::string(path.begin(), path.end()).c_str(), 0, &m_face)) {
        return false;
    }

    m_size = static_cast<int>(size);
    FT_Set_Pixel_Sizes(m_face, 0, m_size);
    return true;
}

void FreetypeFont::RenderText(const std::wstring& text, int x, int y, const Color& color) {
    if (!m_face) return;

    int pen_x = x;
    int pen_y = y;

    for (wchar_t ch : text) {
        if (FT_Load_Char(m_face, ch, FT_LOAD_RENDER)) {
            continue;
        }

        FT_GlyphSlot slot = m_face->glyph;
        FT_Bitmap& bitmap = slot->bitmap;

        for (unsigned int row = 0; row < bitmap.rows; ++row) {
            for (unsigned int col = 0; col < bitmap.width; ++col) {
                unsigned char alpha = bitmap.buffer[row * bitmap.width + col];
                if (alpha > 0) {
                    int px = pen_x + col + slot->bitmap_left;
                    int py = pen_y + row - slot->bitmap_top + m_size;
                    
                    // 混合颜色
                    uint8_t r = (color.r * alpha) / 255;
                    uint8_t g = (color.g * alpha) / 255;
                    uint8_t b = (color.b * alpha) / 255;
                    uint8_t a = (color.a * alpha) / 255;
                    
                    m_window->SetPixel(px, py, b, g, r, a);
                }
            }
        }

        pen_x += slot->advance.x >> 6;
        pen_y += slot->advance.y >> 6;
    }
}

int FreetypeFont::GetTextWidth(const std::wstring& text) const {
    if (!m_face) return 0;

    int width = 0;
    for (wchar_t ch : text) {
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

} // namespace ui 