#pragma once
#include <memory>
#include "core/types.h"
#include "graphics/pixel.h"
#include "graphics/bitmap.h"
#include "core/event.h"

// Surface配置
struct SurfaceConfig {
    int width;                      // 宽度
    int height;                     // 高度
    PixelFormat format;            // 像素格式
    int bufferCount;               // 缓冲区数量(1=单缓冲,2=双缓冲,3=三缓冲)
    bool vsyncEnabled;             // 是否启用垂直同步
};

// Surface接口
class Surface {
public:
    virtual ~Surface() = default;
    
    // 初始化与清理
    virtual bool initialize() = 0;
    virtual void destroy() = 0;
    
    // 缓冲区操作
    virtual Bitmap* lockBuffer() = 0;     // 锁定并获取当前缓冲区
    virtual void unlockBuffer() = 0;      // 解锁当前缓冲区
    virtual void present() = 0;           // 显示当前缓冲区
    
    // 显示控制
    virtual void waitVSync() = 0;          // 等待垂直同步信号
    virtual void setVSyncEnabled(bool enabled) = 0;  // 启用/禁用垂直同步
    
    // 属性访问
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual PixelFormat getPixelFormat() const = 0;
    virtual int getBufferCount() const = 0;
    virtual bool isVSyncEnabled() const = 0;
    virtual PixelLayout getPixelLayout() const = 0;
    virtual BufferLayout getBufferLayout() const = 0;
    
    // 事件处理
    virtual bool pollEvent(Event& event) = 0;
    
    // 工厂方法
    static std::unique_ptr<Surface> create(const SurfaceConfig& config);
}; 