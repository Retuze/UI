#include "graphics/dpi_manager.h"

DPIManager& DPIManager::getInstance() {
    static DPIManager instance;
    return instance;
}

void DPIManager::initialize() {
    HDC hdc = GetDC(NULL);
    dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
    
    scaleFactorX = dpiX / 96.0f;
    scaleFactorY = dpiY / 96.0f;
}

int DPIManager::scaleX(int value) const {
    return static_cast<int>(value * scaleFactorX);
}

int DPIManager::scaleY(int value) const {
    return static_cast<int>(value * scaleFactorY);
} 