#pragma once

class Gravity {
public:
    static constexpr int Left = 0x1;
    static constexpr int Right = 0x2;
    static constexpr int Center = 0x4;
    static constexpr int Top = 0x10;
    static constexpr int Bottom = 0x20;
    static constexpr int VCenter = 0x40;
    
    static constexpr int HORIZONTAL_GRAVITY_MASK = Left | Right | Center;
    static constexpr int VERTICAL_GRAVITY_MASK = Top | Bottom | VCenter;
}; 