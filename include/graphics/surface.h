#pragma once
#include "core/types.h"
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
    
    // 属性访问
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    
    // 工厂方法
    static std::unique_ptr<Surface> create(int width, int height);
}; 