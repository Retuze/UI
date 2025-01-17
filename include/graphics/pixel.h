#pragma once
#include <cstdint>

// Byte order definition
enum class ByteOrder {
    BigEndian,    // MSB first
    LittleEndian  // LSB first
};

// Pixel layout definitions
enum class PixelLayout {
    Horizontal,   // Pixels are packed horizontally
    Vertical      // Pixels are packed vertically
};

enum class BufferLayout {
    RowMajor,     // Data is stored row by row
    ColumnMajor   // Data is stored column by column
};

// Base pixel format definitions
enum class BasePixelFormat {
    MONO,        // 1-bit monochrome
    RGB565,      // 16-bit RGB (5-6-5)
    RGB888,      // 24-bit RGB (8-8-8)
    RGBA8888,    // 32-bit RGBA (8-8-8-8)
    BGRA8888,    // 32-bit BGRA (8-8-8-8)
    A8           // 8-bit alpha
};

// Color definition
struct Color {
    uint8_t r, g, b, a;
    
    constexpr Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}

    // Predefined colors
    static constexpr Color White() { return Color{255, 255, 255}; }
    static constexpr Color Black() { return Color{0, 0, 0}; }
    static constexpr Color Gray() { return Color{128, 128, 128}; }
    static constexpr Color LightGray() { return Color{192, 192, 192}; }
    static constexpr Color DarkGray() { return Color{64, 64, 64}; }
    static constexpr Color Transparent() { return Color{0, 0, 0, 0}; }

    // Format conversion methods
    constexpr uint8_t toMono() const {
        // Using standard luminance formula: Y = 0.299R + 0.587G + 0.114B
        // Integer approximation: (R*77 + G*150 + B*29) >> 8
        uint16_t luminance = (r * 77 + g * 150 + b * 29) >> 8;
        return luminance >= 128 ? 1 : 0;
    }

    constexpr uint32_t toBGRA8888(ByteOrder order = ByteOrder::LittleEndian) const {
        return order == ByteOrder::BigEndian ? 
            (b << 24) | (g << 16) | (r << 8) | a :
            (a << 24) | (r << 16) | (g << 8) | b;
    }

    constexpr uint32_t toRGBA8888(ByteOrder order = ByteOrder::LittleEndian) const {
        return order == ByteOrder::BigEndian ? 
            (r << 24) | (g << 16) | (b << 8) | a :
            (a << 24) | (b << 16) | (g << 8) | r;
    }

    constexpr uint32_t toRGB888(ByteOrder order = ByteOrder::LittleEndian) const {
        return order == ByteOrder::BigEndian ? 
            (r << 16) | (g << 8) | b :
            (b << 16) | (g << 8) | r;
    }

    constexpr uint16_t toRGB565(ByteOrder order = ByteOrder::LittleEndian) const {
        uint16_t value = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
        return order == ByteOrder::BigEndian ? value : 
            ((value & 0xFF) << 8) | ((value >> 8) & 0xFF);
    }

    constexpr uint8_t toA8() const { return a; }
};

// Pixel format configuration
struct PixelFormat {
    BasePixelFormat baseFormat;
    ByteOrder byteOrder;
    PixelLayout pixelLayout;
    BufferLayout bufferLayout;
    
    // Common format presets
    static constexpr PixelFormat MONO_HMSB() {
        return {BasePixelFormat::MONO, ByteOrder::BigEndian, 
                PixelLayout::Horizontal, BufferLayout::RowMajor};
    }
    
    static constexpr PixelFormat MONO_HLSB() {
        return {BasePixelFormat::MONO, ByteOrder::LittleEndian, 
                PixelLayout::Horizontal, BufferLayout::RowMajor};
    }
    
    static constexpr PixelFormat RGB565_BE() {
        return {BasePixelFormat::RGB565, ByteOrder::BigEndian, 
                PixelLayout::Horizontal, BufferLayout::RowMajor};
    }
    
    static constexpr PixelFormat RGB565_LE() {
        return {BasePixelFormat::RGB565, ByteOrder::LittleEndian, 
                PixelLayout::Horizontal, BufferLayout::RowMajor};
    }
    
    static constexpr PixelFormat RGBA8888_BE() {
        return {BasePixelFormat::RGBA8888, ByteOrder::BigEndian, 
                PixelLayout::Horizontal, BufferLayout::RowMajor};
    }
    
    static constexpr PixelFormat RGBA8888_LE() {
        return {BasePixelFormat::RGBA8888, ByteOrder::LittleEndian, 
                PixelLayout::Horizontal, BufferLayout::RowMajor};
    }

    static constexpr PixelFormat RGB888_LE() {
        return {BasePixelFormat::RGB888, ByteOrder::LittleEndian, 
                PixelLayout::Horizontal, BufferLayout::RowMajor};
    }
    
    static constexpr PixelFormat RGB888_BE() {
        return {BasePixelFormat::RGB888, ByteOrder::BigEndian, 
                PixelLayout::Horizontal, BufferLayout::RowMajor};
    }
    
    static constexpr PixelFormat BGRA8888_LE() {
        return {BasePixelFormat::BGRA8888, ByteOrder::LittleEndian, 
                PixelLayout::Horizontal, BufferLayout::RowMajor};
    }
    
    static constexpr PixelFormat BGRA8888_BE() {
        return {BasePixelFormat::BGRA8888, ByteOrder::BigEndian, 
                PixelLayout::Horizontal, BufferLayout::RowMajor};
    }

    static constexpr PixelFormat A8_LE() {
        return {BasePixelFormat::A8, ByteOrder::LittleEndian, 
                PixelLayout::Horizontal, BufferLayout::RowMajor};
    }

    // Utility functions
    constexpr int getBitsPerPixel() const {
        switch (baseFormat) {
            case BasePixelFormat::MONO: return 1;
            case BasePixelFormat::RGB565: return 16;
            case BasePixelFormat::RGB888: return 24;
            case BasePixelFormat::RGBA8888: return 32;
            case BasePixelFormat::BGRA8888: return 32;
            case BasePixelFormat::A8: return 8;
            default: return 0;
        }
    }

    constexpr int getBytesPerPixel() const {
        return (getBitsPerPixel() + 7) / 8;
    }
}; 