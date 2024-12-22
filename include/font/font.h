#pragma once
#include <string>
#include <memory>
#include <cstdint>

namespace ui {

struct Color {
    uint8_t r, g, b, a;
    
    Color(uint8_t r_ = 0, uint8_t g_ = 0, uint8_t b_ = 0, uint8_t a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}
};

class IFont {
public:
    virtual ~IFont() = default;
    
    virtual bool LoadFromFile(const std::wstring& path, float size) = 0;
    virtual void RenderText(const std::wstring& text, int x, int y, const Color& color) = 0;
    virtual int GetTextWidth(const std::wstring& text) const = 0;
    virtual int GetTextHeight() const = 0;
};

using FontPtr = std::shared_ptr<IFont>;

} // namespace ui 