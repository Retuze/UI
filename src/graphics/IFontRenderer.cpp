#include "graphics/IFontRenderer.h"
#include "graphics/text_renderer.h"

std::unique_ptr<IFontRenderer> createDefaultFontRenderer() {
    return std::make_unique<TextRenderer>();
} 