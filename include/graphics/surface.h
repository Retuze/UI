#pragma once
#include "core/types.h"
#include "core/event.h"
#include <memory>

// 抽象Surface接口
class Surface {
public:
    virtual ~Surface() = default;
    
    // 基础操作
    virtual bool initialize() = 0;
    virtual void* lock(int* stride) = 0;
    virtual void unlock() = 0;
    virtual void present() = 0;
    virtual bool close() = 0;
    
    // 属性访问
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual PixelFormat getPixelFormat() const = 0;
    
    // 工厂方法
    static std::unique_ptr<Surface> create(int width, int height, 
                                         PixelFormat format = PixelFormat::BGRA8888LE);
    
    // Add this method:
    virtual bool pollEvent(Event& event) = 0;
}; 