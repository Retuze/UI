#pragma once
#include <windows.h>
#include "core/types.h"

class DPIManager {
public:
    static DPIManager& getInstance();
    
    // 初始化DPI管理器
    void initialize();
    
    // DPI缩放相关
    int scaleX(int value) const;
    int scaleY(int value) const;
    float getScaleX() const { return scaleFactorX; }
    float getScaleY() const { return scaleFactorY; }
    
    // 获取原始DPI值
    int getDpiX() const { return dpiX; }
    int getDpiY() const { return dpiY; }
    
    // 添加这些声明
    Point scalePoint(const Point& p) const;
    Size scaleSize(const Size& s) const;
    Rect scaleRect(const Rect& r) const;

private:
    DPIManager() = default;
    
    int dpiX = 96;
    int dpiY = 96;
    float scaleFactorX = 1.0f;
    float scaleFactorY = 1.0f;
}; 