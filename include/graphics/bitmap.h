#pragma once
#include "graphics/pixel.h"
#include <memory>
#include <vector>

class Bitmap {
public:
    // Constructor with format specification
    Bitmap(int width, int height, const PixelFormat& format);
    
    // Copy/move constructors and assignment operators
    Bitmap(const Bitmap& other);
    Bitmap(Bitmap&& other) noexcept;
    Bitmap& operator=(const Bitmap& other);
    Bitmap& operator=(Bitmap&& other) noexcept;
    
    ~Bitmap();

    // Basic properties
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    int getStride() const { return stride_; }
    const PixelFormat& getFormat() const { return format_; }
    
    // Pixel access
    uint8_t* getPixels() { return pixels_.get(); }
    const uint8_t* getPixels() const { return pixels_.get(); }
    
    // Direct pixel manipulation
    void setPixel(int x, int y, const Color& color);
    Color getPixel(int x, int y) const;
    
    // Buffer management
    void setPixels(void* data);
    bool allocate();
    void reset();
    
    // Utility functions
    int getBytesPerPixel() const;
    int getBitsPerPixel() const;
    bool isValid() const { return pixels_ != nullptr; }
    
    // Format conversion
    void convertTo(BasePixelFormat newFormat);
    
    // 添加新的批量访问接口
    class PixelAccessor {
    public:
        ~PixelAccessor() { if (bitmap) bitmap->endPixelAccess(); }
        
        // 直接访问像素数据
        uint8_t* getRow(int y) { return bitmap->pixels_.get() + y * bitmap->stride_; }
        const uint8_t* getRow(int y) const { return bitmap->pixels_.get() + y * bitmap->stride_; }
        
        // 获取关键参数
        int getBytesPerPixel() const { return bitmap->getBytesPerPixel(); }
        int getStride() const { return bitmap->stride_; }
        
    private:
        friend class Bitmap;
        PixelAccessor(Bitmap* bmp) : bitmap(bmp) {}
        Bitmap* bitmap;
    };
    
    // 开始批量像素访问
    PixelAccessor beginPixelAccess() {
        return PixelAccessor(this);
    }

private:
    int width_;
    int height_;
    int stride_;
    PixelFormat format_;
    std::unique_ptr<uint8_t[]> pixels_;
    
    // Helper methods
    void calculateStride();
    size_t getOffset(int x, int y) const;
    void endPixelAccess() {} // 可以在这里添加必要的清理工作
}; 