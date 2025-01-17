#include "graphics/harfbuzz_wrapper.h"

hb_font_t* HarfBuzzWrapper::createFont(FT_Face ftFace) {
    if (!ftFace) return nullptr;
    return hb_ft_font_create(ftFace, nullptr);
}

void HarfBuzzWrapper::destroyFont(hb_font_t* font) {
    if (font) {
        hb_font_destroy(font);
    }
}

std::vector<IFontRenderer::ShapedGlyph> HarfBuzzWrapper::shapeText(
    hb_font_t* font,
    const std::string& text,
    const TextStyle& style) {
    
    std::vector<IFontRenderer::ShapedGlyph> result;
    if (!font || text.empty()) {
        return result;
    }

    // 创建并配置HarfBuzz缓冲区
    hb_buffer_t* buffer = hb_buffer_create();
    if (!buffer) return result;
    
    // 添加文本到缓冲区
    hb_buffer_add_utf8(buffer, text.c_str(), -1, 0, -1);
    
    // 设置文本方向
    hb_direction_t direction = (style.direction == TextStyle::TextDirection::RTL) 
        ? HB_DIRECTION_RTL 
        : HB_DIRECTION_LTR;
    hb_buffer_set_direction(buffer, direction);
    
    // 设置文字系统和语言
    hb_buffer_set_script(buffer, hb_script_from_string(style.script.c_str(), -1));
    hb_buffer_set_language(buffer, hb_language_from_string(style.language.c_str(), -1));
    
    // 执行文本整形
    hb_shape(font, buffer, nullptr, 0);
    
    // 获取整形结果
    unsigned int glyphCount;
    hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
    hb_glyph_position_t* glyphPos = hb_buffer_get_glyph_positions(buffer, &glyphCount);
    
    if (glyphInfo && glyphPos) {
        // 转换HarfBuzz结果到我们的格式
        result.reserve(glyphCount);
        for (unsigned int i = 0; i < glyphCount; i++) {
            IFontRenderer::ShapedGlyph glyph;
            glyph.glyphId = glyphInfo[i].codepoint;
            // HarfBuzz使用26.6固定小数点格式，需要除以64转换为浮点数
            glyph.x_advance = glyphPos[i].x_advance / 64.0f;
            glyph.y_advance = glyphPos[i].y_advance / 64.0f;
            glyph.x_offset = glyphPos[i].x_offset / 64.0f;
            glyph.y_offset = glyphPos[i].y_offset / 64.0f;
            result.push_back(glyph);
        }
    }
    
    // 清理资源
    hb_buffer_destroy(buffer);
    
    return result;
}