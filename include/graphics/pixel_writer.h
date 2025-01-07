#pragma once
#include "core/types.h"

class PixelWriter {
public:
    virtual ~PixelWriter() = default;
    virtual void writePixel(void* dst, Color color) const = 0;
    virtual int getBytesPerPixel() const = 0;
    
    static std::unique_ptr<PixelWriter> create(PixelFormat format);
};

template<PixelFormat Format>
class ConcretePixelWriter : public PixelWriter {
public:
    void writePixel(void* dst, Color color) const override {
        if constexpr (Format == PixelFormat::RGBA8888BE) {
            *static_cast<uint32_t*>(dst) = color.toRGBA8888BE();
        } else if constexpr (Format == PixelFormat::BGRA8888LE) {
            *static_cast<uint32_t*>(dst) = color.toBGRA8888LE();
        } else if constexpr (Format == PixelFormat::RGB888BE) {
            auto value = color.toRGB888BE();
            auto* p = static_cast<uint8_t*>(dst);
            p[0] = (value >> 16) & 0xFF;
            p[1] = (value >> 8) & 0xFF;
            p[2] = value & 0xFF;
        } else if constexpr (Format == PixelFormat::RGB888LE) {
            auto value = color.toRGB888LE();
            auto* p = static_cast<uint8_t*>(dst);
            p[0] = value & 0xFF;
            p[1] = (value >> 8) & 0xFF;
            p[2] = (value >> 16) & 0xFF;
        } else if constexpr (Format == PixelFormat::RGB565BE) {
            *static_cast<uint16_t*>(dst) = color.toRGB565BE();
        } else if constexpr (Format == PixelFormat::RGB565LE) {
            *static_cast<uint16_t*>(dst) = color.toRGB565LE();
        } else if constexpr (Format == PixelFormat::A8BE || Format == PixelFormat::A8LE) {
            *static_cast<uint8_t*>(dst) = color.toA8BE();
        }
    }
    
    int getBytesPerPixel() const override {
        if constexpr (Format == PixelFormat::RGBA8888BE || 
                     Format == PixelFormat::BGRA8888LE ||
                     Format == PixelFormat::BGRA8888BE ||
                     Format == PixelFormat::RGBA8888LE) {
            return 4;
        } else if constexpr (Format == PixelFormat::RGB888BE ||
                            Format == PixelFormat::RGB888LE) {
            return 3;
        } else if constexpr (Format == PixelFormat::RGB565BE ||
                            Format == PixelFormat::RGB565LE) {
            return 2;
        } else if constexpr (Format == PixelFormat::A8BE ||
                            Format == PixelFormat::A8LE) {
            return 1;
        }
        return 0; // Unknown format
    }
}; 