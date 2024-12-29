#include "graphics/dpi_manager.h"

DPIManager& DPIManager::getInstance() {
    static DPIManager instance;
    return instance;
}

void DPIManager::initialize() {
    #ifdef _WIN32
        HWND hwnd = GetDesktopWindow();
        HDC hdc = GetDC(hwnd);
        dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
        dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(hwnd, hdc);
    #else
        dpiX = dpiY = 96;
    #endif
    
    scaleFactorX = dpiX / 96.0f;
    scaleFactorY = dpiY / 96.0f;
}

Point DPIManager::scalePoint(const Point& p) const {
    return Point(scaleX(p.x), scaleY(p.y));
}

Size DPIManager::scaleSize(const Size& s) const {
    return Size(scaleX(s.width), scaleY(s.height));
}

Rect DPIManager::scaleRect(const Rect& r) const {
    return Rect(scaleX(r.x), scaleY(r.y), 
                scaleX(r.width), scaleY(r.height));
}

int DPIManager::scaleX(int value) const {
    return static_cast<int>(value * scaleFactorX);
}

int DPIManager::scaleY(int value) const {
    return static_cast<int>(value * scaleFactorY);
} 