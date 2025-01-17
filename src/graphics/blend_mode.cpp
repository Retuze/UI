#include "graphics/blend_mode.h"

Color blendColors(const Color& src, const Color& dst, BlendMode mode) {
    float sa = src.a / 255.0f;
    float sr = src.r / 255.0f;
    float sg = src.g / 255.0f;
    float sb = src.b / 255.0f;
    
    float da = dst.a / 255.0f;
    float dr = dst.r / 255.0f;
    float dg = dst.g / 255.0f;
    float db = dst.b / 255.0f;
    
    float ra, rr, rg, rb;
    
    switch (mode) {
        case BlendMode::SrcOver:
            ra = sa + da * (1 - sa);
            rr = (sr * sa + dr * da * (1 - sa)) / ra;
            rg = (sg * sa + dg * da * (1 - sa)) / ra;
            rb = (sb * sa + db * da * (1 - sa)) / ra;
            break;
            
        case BlendMode::Screen:
            ra = sa + da - sa * da;
            rr = sr + dr - sr * dr;
            rg = sg + dg - sg * dg;
            rb = sb + db - sb * db;
            break;
            
        // 可以添加更多混合模式...
            
        default:
            return src;
    }
    
    return Color(
        static_cast<uint8_t>(rr * 255),
        static_cast<uint8_t>(rg * 255),
        static_cast<uint8_t>(rb * 255),
        static_cast<uint8_t>(ra * 255)
    );
} 