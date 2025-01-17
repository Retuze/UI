#pragma once
#include "core/types.h"
#include "graphics/bitmap.h"
#include <string>
#include <memory>

// 基础文本样式
struct TextStyle {
    std::string fontName = "default";
    int size = 16;
    Color color;
    enum class TextDirection {
        LTR, RTL
    } direction = TextDirection::LTR;
    
    std::string language = "en";
    std::string script = "Latn";
};

// 字体渲染器接口
class IFontRenderer {
public:
    // 添加字形度量信息结构
    struct GlyphMetrics {
        int width;          // 字形宽度
        int height;         // 字形高度
        int horiBearingX;   // 水平方向的起点偏移
        int horiBearingY;   // 垂直方向的起点偏移
        int horiAdvance;    // 水平方向的前进值
        int vertBearingX;   // 垂直排版时的X偏移
        int vertBearingY;   // 垂直排版时的Y偏移
        int vertAdvance;    // 垂直方向的前进值
    };

    struct ShapedGlyph {
        uint32_t glyphId;
        float x_advance;
        float y_advance;
        float x_offset;
        float y_offset;
    };

    virtual ~IFontRenderer() = default;
    
    virtual bool loadFont(const std::string& fontPath, 
                         const std::string& name = "default") = 0;
    
    virtual void renderText(Bitmap* bitmap,
                          const std::string& text, 
                          const TextStyle& style,
                          int x, int y) = 0;
                          
    virtual Size getTextSize(const std::string& text,
                           const TextStyle& style) = 0;
};

std::unique_ptr<IFontRenderer> createDefaultFontRenderer(); 