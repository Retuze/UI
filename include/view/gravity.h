#pragma once

struct Gravity {
    static constexpr int Left = 0x1;
    static constexpr int Right = 0x2;
    static constexpr int Center = 0x4;
    static constexpr int Top = 0x8;
    static constexpr int Bottom = 0x10;
    static constexpr int VCenter = 0x20;
}; 