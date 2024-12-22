#pragma once
#include "font/font.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace ui {

class FreetypeFont : public IFont {
public:
    FreetypeFont(class IWindow* window);
    ~FreetypeFont() override;

    bool LoadFromFile(const std::wstring& path, float size) override;
    void RenderText(const std::wstring& text, int x, int y, const Color& color) override;
    int GetTextWidth(const std::wstring& text) const override;
    int GetTextHeight() const override;

private:
    FT_Library m_ft;
    FT_Face m_face;
    class IWindow* m_window;
    int m_size;
};

} // namespace ui 