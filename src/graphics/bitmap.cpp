#include "graphics/bitmap.h"
#include "core/logger.h"
#include <algorithm>
#include <cstring>

LOG_TAG("Bitmap");

Bitmap::Bitmap(int width, int height, const PixelFormat& format)
    : width_(width)
    , height_(height)
    , format_(format)
    , stride_(width * format.getBytesPerPixel()) {
    allocate();
}

Bitmap::Bitmap(const Bitmap& other)
    : width_(other.width_)
    , height_(other.height_)
    , stride_(other.stride_)
    , format_(other.format_) {
    if (other.pixels_) {
        size_t size = stride_ * height_;
        pixels_ = std::make_unique<uint8_t[]>(size);
        std::memcpy(pixels_.get(), other.pixels_.get(), size);
    }
}

Bitmap::Bitmap(Bitmap&& other) noexcept
    : width_(other.width_)
    , height_(other.height_)
    , stride_(other.stride_)
    , format_(other.format_)
    , pixels_(std::move(other.pixels_)) {
    other.width_ = 0;
    other.height_ = 0;
    other.stride_ = 0;
}

Bitmap& Bitmap::operator=(const Bitmap& other) {
    if (this != &other) {
        width_ = other.width_;
        height_ = other.height_;
        stride_ = other.stride_;
        format_ = other.format_;
        
        if (other.pixels_) {
            size_t size = stride_ * height_;
            pixels_ = std::make_unique<uint8_t[]>(size);
            std::memcpy(pixels_.get(), other.pixels_.get(), size);
        } else {
            pixels_.reset();
        }
    }
    return *this;
}

Bitmap& Bitmap::operator=(Bitmap&& other) noexcept {
    if (this != &other) {
        width_ = other.width_;
        height_ = other.height_;
        stride_ = other.stride_;
        format_ = other.format_;
        pixels_ = std::move(other.pixels_);
        
        other.width_ = 0;
        other.height_ = 0;
        other.stride_ = 0;
    }
    return *this;
}

Bitmap::~Bitmap() = default;

void Bitmap::calculateStride() {
    stride_ = width_ * format_.getBytesPerPixel();
}

bool Bitmap::allocate() {
    if (width_ <= 0 || height_ <= 0) {
        LOGE("Invalid dimensions: %dx%d", width_, height_);
        return false;
    }
    
    size_t size = stride_ * height_;
    try {
        pixels_ = std::make_unique<uint8_t[]>(size);
        std::memset(pixels_.get(), 0, size);
        return true;
    } catch (const std::bad_alloc& e) {
        LOGE("Failed to allocate bitmap memory: %s", e.what());
        return false;
    }
}

void Bitmap::reset() {
    pixels_.reset();
    width_ = 0;
    height_ = 0;
    stride_ = 0;
}

void Bitmap::setPixels(void* data) {
    if (!data) {
        pixels_.reset();
        return;
    }
    
    if (!pixels_) {
        allocate();
    }
    
    if (pixels_) {
        std::memcpy(pixels_.get(), data, stride_ * height_);
    }
}

int Bitmap::getBytesPerPixel() const {
    return format_.getBytesPerPixel();
}

int Bitmap::getBitsPerPixel() const {
    return format_.getBitsPerPixel();
}

size_t Bitmap::getOffset(int x, int y) const {
    if (format_.bufferLayout == BufferLayout::RowMajor) {
        return y * stride_ + x * getBytesPerPixel();
    } else {
        return x * stride_ + y * getBytesPerPixel();
    }
}

void Bitmap::setPixel(int x, int y, const Color& color) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_ || !pixels_) {
        return;
    }
    
    size_t offset = getOffset(x, y);
    uint8_t* pixel = pixels_.get() + offset;
    
    switch (format_.baseFormat) {
        case BasePixelFormat::RGBA8888:
            *reinterpret_cast<uint32_t*>(pixel) = color.toRGBA8888();
            break;
        case BasePixelFormat::BGRA8888:
            *reinterpret_cast<uint32_t*>(pixel) = color.toBGRA8888();
            break;
        case BasePixelFormat::RGB888: {
            pixel[0] = color.r;
            pixel[1] = color.g;
            pixel[2] = color.b;
            break;
        }
        case BasePixelFormat::RGB565:
            *reinterpret_cast<uint16_t*>(pixel) = color.toRGB565();
            break;
        case BasePixelFormat::MONO:
            // Set bit in byte based on x position
            if (color.toMono()) {
                pixels_[offset + (x >> 3)] |= (1 << (7 - (x & 7)));
            } else {
                pixels_[offset + (x >> 3)] &= ~(1 << (7 - (x & 7)));
            }
            break;
        case BasePixelFormat::A8:
            pixel[0] = color.a;
            break;
    }
}

Color Bitmap::getPixel(int x, int y) const {
    if (x < 0 || x >= width_ || y < 0 || y >= height_ || !pixels_) {
        return Color::Transparent();
    }
    
    size_t offset = getOffset(x, y);
    const uint8_t* pixel = pixels_.get() + offset;
    
    switch (format_.baseFormat) {
        case BasePixelFormat::RGBA8888: {
            uint32_t value = *reinterpret_cast<const uint32_t*>(pixel);
            return Color(
                (value >> 24) & 0xFF,
                (value >> 16) & 0xFF,
                (value >> 8) & 0xFF,
                value & 0xFF
            );
        }
        case BasePixelFormat::BGRA8888: {
            uint32_t value = *reinterpret_cast<const uint32_t*>(pixel);
            return Color(
                (value >> 16) & 0xFF,
                (value >> 8) & 0xFF,
                value & 0xFF,
                (value >> 24) & 0xFF
            );
        }
        case BasePixelFormat::RGB888:
            return Color(pixel[0], pixel[1], pixel[2]);
        case BasePixelFormat::RGB565: {
            uint16_t value = *reinterpret_cast<const uint16_t*>(pixel);
            return Color(
                ((value >> 11) & 0x1F) << 3,
                ((value >> 5) & 0x3F) << 2,
                (value & 0x1F) << 3
            );
        }
        case BasePixelFormat::MONO: {
            uint8_t byte = pixels_[offset + (x >> 3)];
            bool bit = byte & (1 << (7 - (x & 7)));
            return bit ? Color::White() : Color::Black();
        }
        case BasePixelFormat::A8:
            return Color(0, 0, 0, pixel[0]);
        default:
            return Color::Transparent();
    }
} 