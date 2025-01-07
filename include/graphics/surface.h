#pragma once
#include <memory>
#include "core/types.h"
#include "core/event.h"

// Surface缓冲区配置
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
    virtual void* dequeueBuffer() = 0;     // 获取空闲缓冲区
    virtual bool queueBuffer() = 0;        // 提交渲染完成的缓冲区
    virtual void present() = 0;            // 显示队列中最早的缓冲区
    
    // 显示控制
    virtual void waitVSync() = 0;          // 等待垂直同步信号
    virtual void setVSyncEnabled(bool enabled) = 0;  // 启用/禁用垂直同步
    
    // 属性访问
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual PixelFormat getPixelFormat() const = 0;
    virtual int getBufferCount() const = 0;
    virtual bool isVSyncEnabled() const = 0;
    
    // 事件处理
    virtual bool pollEvent(Event& event) = 0;
    
    // 工厂方法
    static std::unique_ptr<Surface> create(const SurfaceConfig& config);
}; 