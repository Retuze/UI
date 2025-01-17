#include "graphics/matrix.h"

Matrix Matrix::makeIdentity() {
    return Matrix();
}

Matrix Matrix::makeTranslate(float dx, float dy) {
    Matrix matrix;
    matrix.m[2] = dx;  // x translation
    matrix.m[5] = dy;  // y translation
    return matrix;
}

Matrix Matrix::makeScale(float sx, float sy) {
    Matrix matrix;
    matrix.m[0] = sx;  // x scale
    matrix.m[4] = sy;  // y scale
    return matrix;
}

Matrix Matrix::makeRotate(float degrees) {
    float radians = degrees * kPI / 180.0f;
    float s = std::sin(radians);
    float c = std::cos(radians);
    
    Matrix matrix;
    matrix.m[0] = c;
    matrix.m[1] = -s;
    matrix.m[3] = s;
    matrix.m[4] = c;
    return matrix;
}

Matrix Matrix::operator*(const Matrix& other) const {
    Matrix result;
    
    // 3x3 矩阵乘法
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            float sum = 0;
            for (int k = 0; k < 3; k++) {
                sum += m[i * 3 + k] * other.m[k * 3 + j];
            }
            result.m[i * 3 + j] = sum;
        }
    }
    
    return result;
}

Point Matrix::mapPoint(const Point& point) const {
    // 将点转换为齐次坐标
    float x = point.x;
    float y = point.y;
    float w = 1;
    
    // 应用变换
    float x_ = m[0] * x + m[1] * y + m[2] * w;
    float y_ = m[3] * x + m[4] * y + m[5] * w;
    float w_ = m[6] * x + m[7] * y + m[8] * w;
    
    // 转回笛卡尔坐标
    if (w_ != 0) {
        x_ /= w_;
        y_ /= w_;
    }
    
    return Point(x_, y_);
}

Rect Matrix::mapRect(const Rect& rect) const {
    // 变换四个角点
    Point p1 = mapPoint(Point(rect.x, rect.y));
    Point p2 = mapPoint(Point(rect.x + rect.width, rect.y));
    Point p3 = mapPoint(Point(rect.x, rect.y + rect.height));
    Point p4 = mapPoint(Point(rect.x + rect.width, rect.y + rect.height));
    
    // 计算包围盒
    float minX = std::min({p1.x, p2.x, p3.x, p4.x});
    float minY = std::min({p1.y, p2.y, p3.y, p4.y});
    float maxX = std::max({p1.x, p2.x, p3.x, p4.x});
    float maxY = std::max({p1.y, p2.y, p3.y, p4.y});
    
    return Rect(minX, minY, maxX - minX, maxY - minY);
} 