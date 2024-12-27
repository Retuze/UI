#include "graphics/gdi_renderer.h"
#include <algorithm>

//------------------------------------------------------------------------------
// 构造函数和析构函数
//------------------------------------------------------------------------------

GDIRenderer::GDIRenderer(HWND hwnd) : hwnd(hwnd), width(0), height(0), 
    hdc(nullptr), hdcMem(nullptr), hBitmap(nullptr) {
    if (hwnd) {
        initialize(hwnd);
    }
}

GDIRenderer::~GDIRenderer() {
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);
}

//------------------------------------------------------------------------------
// 窗口管理相关
//------------------------------------------------------------------------------

ATOM GDIRenderer::registerClass() {
    static bool registered = false;
    if (registered) return true;

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"SimpleGUI";
    
    registered = RegisterClassEx(&wc);
    return registered;
}

bool GDIRenderer::createWindow(int width, int height, const char* title) {
    if (!registerClass()) return false;

    // 根据 DPI 调整窗口大小
    HDC hdc = GetDC(NULL);
    int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
    
    width = MulDiv(width, dpiX, 96);
    height = MulDiv(height, dpiY, 96);
    
    RECT rc = {0, 0, width, height};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    
    // 转换标题为宽字符
    int titleLen = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
    std::wstring wTitle(titleLen, 0);
    MultiByteToWideChar(CP_UTF8, 0, title, -1, &wTitle[0], titleLen);
    
    hwnd = CreateWindowW(
        L"SimpleGUI", wTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );
    
    if (!hwnd) return false;
    
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    return initialize(hwnd);
}

//------------------------------------------------------------------------------
// 事件处理
//------------------------------------------------------------------------------

bool GDIRenderer::processEvents() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return !shouldClose;
}

LRESULT CALLBACK GDIRenderer::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    GDIRenderer* renderer = (GDIRenderer*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (msg) {
        case WM_PAINT:
            if (renderer) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                renderer->present();
                EndPaint(hwnd, &ps);
            }
            return 0;
            
        case WM_CLOSE:
            if (renderer) renderer->shouldClose = true;
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_LBUTTONDOWN: {
            Event event(EventType::MousePress);
            event.position = Point(LOWORD(lParam), HIWORD(lParam));
            event.button = 1;
            if (renderer && renderer->eventHandler) {
                renderer->eventHandler(event);
            }
            return 0;
        }
        
        case WM_LBUTTONUP: {
            Event event(EventType::MouseRelease);
            event.position = Point(LOWORD(lParam), HIWORD(lParam));
            event.button = 1;
            if (renderer && renderer->eventHandler) {
                renderer->eventHandler(event);
            }
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            Event event(EventType::MouseMove);
            event.position = Point(LOWORD(lParam), HIWORD(lParam));
            if (renderer && renderer->eventHandler) {
                renderer->eventHandler(event);
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
// 基础像素操作
//------------------------------------------------------------------------------

void GDIRenderer::setPixel(int x, int y, Color color) {
    if (!clipStack.empty()) {
        const Rect& clip = clipStack.back();
        if (x < clip.x || x >= clip.x + clip.width ||
            y < clip.y || y >= clip.y + clip.height) {
            return;
        }
    }
    SetPixel(hdcMem, x, y, RGB(color.r, color.g, color.b));
}

Color GDIRenderer::getPixel(int x, int y) const {
    COLORREF pixel = GetPixel(hdcMem, x, y);
    return Color(
        GetRValue(pixel),
        GetGValue(pixel),
        GetBValue(pixel)
    );
}

//------------------------------------------------------------------------------
// 内存块操作
//------------------------------------------------------------------------------

void GDIRenderer::blitRaw(int x, int y, const void* data, int width, int height, 
                         int stride, PixelFormat format) {
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // 负值表示从上到下
    bmi.bmiHeader.biPlanes = 1;
    
    switch (format) {
        case PixelFormat::RGB565:
            bmi.bmiHeader.biBitCount = 16;
            break;
        case PixelFormat::RGB888:
            bmi.bmiHeader.biBitCount = 24;
            break;
        case PixelFormat::RGBA8888:
            bmi.bmiHeader.biBitCount = 32;
            break;
        case PixelFormat::A8:
            bmi.bmiHeader.biBitCount = 8;
            break;
    }
    
    SetDIBitsToDevice(hdcMem, x, y, width, height, 
                      0, 0, 0, height, 
                      data, &bmi, DIB_RGB_COLORS);
}

void GDIRenderer::copyRect(const Rect& src, const Rect& dst) {
    BitBlt(hdcMem, dst.x, dst.y, dst.width, dst.height,
           hdcMem, src.x, src.y, SRCCOPY);
}

//------------------------------------------------------------------------------
// 基础绘图操作
//------------------------------------------------------------------------------

void GDIRenderer::drawLine(int x1, int y1, int x2, int y2, Color color) {
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(color.r, color.g, color.b));
    HPEN oldPen = (HPEN)SelectObject(hdcMem, pen);
    
    MoveToEx(hdcMem, x1, y1, NULL);
    LineTo(hdcMem, x2, y2);
    
    SelectObject(hdcMem, oldPen);
    DeleteObject(pen);
}

void GDIRenderer::fillRect(const Rect& rect, Color color) {
    if (!clipStack.empty()) {
        const Rect& clip = clipStack.back();
        Rect r = {
            std::max(rect.x, clip.x),
            std::max(rect.y, clip.y),
            std::min(rect.x + rect.width, clip.x + clip.width) - std::max(rect.x, clip.x),
            std::min(rect.y + rect.height, clip.y + clip.height) - std::max(rect.y, clip.y)
        };
        if (r.width <= 0 || r.height <= 0) return;
        
        RECT winRect = {r.x, r.y, r.x + r.width, r.y + r.height};
        HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
        FillRect(hdcMem, &winRect, brush);
        DeleteObject(brush);
    } else {
        RECT winRect = {rect.x, rect.y, rect.x + rect.width, rect.y + rect.height};
        HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
        FillRect(hdcMem, &winRect, brush);
        DeleteObject(brush);
    }
}

//------------------------------------------------------------------------------
// 缓冲区操作
//------------------------------------------------------------------------------

void GDIRenderer::clear(Color color) {
    RECT rect = {0, 0, width, height};
    HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
    FillRect(hdcMem, &rect, brush);
    DeleteObject(brush);
}

void GDIRenderer::present(const Rect* dirtyRects, int numRects) {
    if (dirtyRects && numRects > 0) {
        for (int i = 0; i < numRects; ++i) {
            const Rect& r = dirtyRects[i];
            BitBlt(hdc, r.x, r.y, r.width, r.height,
                   hdcMem, r.x, r.y, SRCCOPY);
        }
    } else {
        BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
    }
}

//------------------------------------------------------------------------------
// 剪切区域操作
//------------------------------------------------------------------------------

void GDIRenderer::pushClipRect(const Rect& rect) {
    HRGN clipRegion;
    if (!clipStack.empty()) {
        const Rect& current = clipStack.back();
        Rect newClip = {
            std::max(rect.x, current.x),
            std::max(rect.y, current.y),
            std::min(rect.x + rect.width, current.x + current.width) - std::max(rect.x, current.x),
            std::min(rect.y + rect.height, current.y + current.height) - std::max(rect.y, current.y)
        };
        clipStack.push_back(newClip);
        clipRegion = CreateRectRgn(newClip.x, newClip.y, 
                                  newClip.x + newClip.width, 
                                  newClip.y + newClip.height);
    } else {
        clipStack.push_back(rect);
        clipRegion = CreateRectRgn(rect.x, rect.y, 
                                  rect.x + rect.width, 
                                  rect.y + rect.height);
    }
    SelectClipRgn(hdcMem, clipRegion);
    DeleteObject(clipRegion);
}

void GDIRenderer::popClipRect() {
    if (!clipStack.empty()) {
        clipStack.pop_back();
        if (!clipStack.empty()) {
            const Rect& current = clipStack.back();
            HRGN clipRegion = CreateRectRgn(current.x, current.y,
                                           current.x + current.width,
                                           current.y + current.height);
            SelectClipRgn(hdcMem, clipRegion);
            DeleteObject(clipRegion);
        } else {
            SelectClipRgn(hdcMem, NULL);
        }
    }
}

//------------------------------------------------------------------------------
// 内存锁定操作
//------------------------------------------------------------------------------

void* GDIRenderer::lockRect(const Rect& rect, int* stride) {
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = rect.width;
    bmi.bmiHeader.biHeight = -rect.height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    
    *stride = ((rect.width * 4 + 3) & ~3);
    lockedBits = malloc(*stride * rect.height);
    GetDIBits(hdcMem, hBitmap, rect.y, rect.height, lockedBits, &bmi, DIB_RGB_COLORS);
    return lockedBits;
}

void GDIRenderer::unlockRect() {
    if (lockedBits) {
        free(lockedBits);
        lockedBits = nullptr;
    }
}

//------------------------------------------------------------------------------
// 工厂方法实现
//------------------------------------------------------------------------------

std::unique_ptr<Renderer> Renderer::create(int width, int height, const char* title) {
#ifdef _WIN32
    auto renderer = std::make_unique<GDIRenderer>(nullptr);  // 这里需要修改
    if (renderer->createWindow(width, height, title)) {
        return renderer;
    }
#endif
    return nullptr;
}

bool GDIRenderer::initialize(HWND hwnd) {
    // 启用 DPI 感知
    SetProcessDPIAware();
    
    this->hwnd = hwnd;
    hdc = GetDC(hwnd);
    if (!hdc) return false;
    
    hdcMem = CreateCompatibleDC(hdc);
    if (!hdcMem) {
        ReleaseDC(hwnd, hdc);
        return false;
    }
    
    RECT rect;
    GetClientRect(hwnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    
    hBitmap = CreateCompatibleBitmap(hdc, width, height);
    if (!hBitmap) {
        DeleteDC(hdcMem);
        ReleaseDC(hwnd, hdc);
        return false;
    }
    
    SelectObject(hdcMem, hBitmap);
    
    // 初始化时清空背景
    RECT winRect = {0, 0, width, height};
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdcMem, &winRect, whiteBrush);
    DeleteObject(whiteBrush);
    
    return true;
} 