#pragma once
#include "core/types.h"
#include <array>
#include <cmath>

class Matrix {
public:
    // 3x3矩阵,使用行主序存储
    std::array<float, 9> m = {1, 0, 0,
                             0, 1, 0,
                             0, 0, 1};
                             
    // 工厂方法
    static Matrix makeIdentity();
    static Matrix makeTranslate(float dx, float dy);
    static Matrix makeScale(float sx, float sy);
    static Matrix makeRotate(float degrees);
    
    // 矩阵操作
    Matrix operator*(const Matrix& other) const;
    Point mapPoint(const Point& point) const;
    Rect mapRect(const Rect& rect) const;
    
private:
    static constexpr float kPI = 3.14159265358979323846f;
}; 