#include "graphics/surface.h"
#include "platform/win32/win32_surface.h"
#include <memory>

std::unique_ptr<Surface> Surface::create(const SurfaceConfig& config) {
    #ifdef _WIN32
        return std::make_unique<Win32Surface>(config);
    #else
        #error "Platform not supported"
    #endif
} 