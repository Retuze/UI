#pragma once
#include "font/font.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace ui {

class FreetypeFont : public IFont {
public:
    FreetypeFont(class IWindow* window);
    ~FreetypeFont() override;

    bool LoadFromFile(const char* path, float size) override;
    void RenderText(const char* text, int x, int y, const Color& color) override;
    int GetTextWidth(const char* text) const override;
    int GetTextHeight() const override;

private:
    FT_Library m_ft;
    FT_Face m_face;
    class IWindow* m_window;
    int m_size;

    static std::wstring Utf8ToWide(const char* utf8Str);
    static std::string WideToUtf8(const std::wstring& wideStr);
};

} // namespace ui 