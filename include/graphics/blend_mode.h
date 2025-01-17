#pragma once
#include "core/types.h"
#include "graphics/pixel.h"

enum class BlendMode {
    Clear,      // 清除
    Src,        // 源
    Dst,        // 目标
    SrcOver,    // 源在上
    DstOver,    // 目标在上
    SrcIn,      // 源在内
    DstIn,      // 目标在内
    SrcOut,     // 源在外
    DstOut,     // 目标在外
    SrcATop,    // 源在顶部
    DstATop,    // 目标在顶部
    Xor,        // 异或
    Plus,       // 加法
    Modulate,   // 正片叠底
    Screen,     // 滤色
    Overlay     // 叠加
};

// 混合两个颜色
Color blendColors(const Color& src, const Color& dst, BlendMode mode); 