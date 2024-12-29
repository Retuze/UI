#include "graphics/gdi_renderer.h"
#include "core/logger.h"
#include <algorithm>
#include <thread>
#include <atomic>
#include <chrono>
#include <graphics/dpi_manager.h>
#include "core/choreographer.h"
#include <windowsx.h>  // For GET_X_LPARAM, GET_Y_LPARAM

LOG_TAG("GDIRenderer");


GDIRenderer::GDIRenderer(HWND hwnd) : hwnd(hwnd), width(0), height(0), 
    hdc(nullptr), hdcMem(nullptr), hBitmap(nullptr) {
    // 初始化DPI管理器
    DPIManager::getInstance().initialize();
    
    if (hwnd) {
        initialize(hwnd);
    }
}

GDIRenderer::~GDIRenderer() {
    // 先停止渲染线程
    running = false;
    if (renderThread.joinable()) {
        renderThread.join();
    }
    
    // 然后释放GDI资源
    if (hBitmap) {
        DeleteObject(hBitmap);
        hBitmap = nullptr;
    }
    if (hdcMem) {
        DeleteDC(hdcMem);
        hdcMem = nullptr;
    }
    if (hdc) {
        ReleaseDC(hwnd, hdc);
        hdc = nullptr;
    }
}


ATOM GDIRenderer::registerClass() {
    static bool registered = false;
    if (registered) return true;

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"SimpleGUI";
    
    registered = RegisterClassEx(&wc);
    return registered;
}

bool GDIRenderer::createWindow(int width, int height, const char* title) {
    if (!registerClass()) return false;

    // 设置DPI感知
    SetProcessDPIAware();
    DPIManager::getInstance().initialize();
    
    // 计算实际需要的客户区大小
    RECT rc = {0, 0, width, height};
    AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW);
    
    // 获取DPI缩放后的窗口尺寸
    auto& dpiManager = DPIManager::getInstance();
    int windowWidth = dpiManager.scaleX(rc.right - rc.left);
    int windowHeight = dpiManager.scaleY(rc.bottom - rc.top);
    
    // 转换标题为宽字符
    int titleLen = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
    std::wstring wTitle(titleLen, 0);
    MultiByteToWideChar(CP_UTF8, 0, title, -1, &wTitle[0], titleLen);
    
    hwnd = CreateWindowW(
        L"SimpleGUI", wTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowWidth, windowHeight,
        NULL, NULL, GetModuleHandle(NULL), this
    );
    
    if (!hwnd) return false;
    
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    bool result = initialize(hwnd);
    if (result) {
        clear(Color(255, 255, 255));
        present();
    }
    return result;
}


bool GDIRenderer::processEvents() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            shouldClose = true;
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return !shouldClose;
}

LRESULT CALLBACK GDIRenderer::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    GDIRenderer* renderer = (GDIRenderer*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (msg) {
        case WM_CLOSE:
            if (renderer) renderer->shouldClose = true;
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MOUSEMOVE: {
            auto& dpiManager = DPIManager::getInstance();
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            
            Event event(msg == WM_MOUSEMOVE ? EventType::MouseMove :
                        msg == WM_LBUTTONDOWN ? EventType::MousePress :
                        EventType::MouseRelease);
            
            event.position = Point(x, y);  // 不需要反向缩放，因为这已经是实际像素坐标
            event.button = 1;
            
            if (renderer && renderer->eventHandler) {
                renderer->eventHandler(event);
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}



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


void GDIRenderer::clear(Color color) {
    RECT rect = {0, 0, width, height};
    HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
    FillRect(hdcMem, &rect, brush);
    DeleteObject(brush);
}

void GDIRenderer::present(const Rect* dirtyRects, int numRects) {
    if (!hdc || !hdcMem) return;
    
    if (dirtyRects && numRects > 0) {
        // Only update dirty regions
        for (int i = 0; i < numRects; ++i) {
            const Rect& r = dirtyRects[i];
            BitBlt(hdc, r.x, r.y, r.width, r.height,
                   hdcMem, r.x, r.y, SRCCOPY);
        }
    } else {
        // Update entire window
        BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
    }
}


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


std::unique_ptr<Renderer> Renderer::create(int width, int height, const char* title) {
#ifdef _WIN32
    auto renderer = std::unique_ptr<GDIRenderer>(new GDIRenderer(nullptr));
    if (renderer->createWindow(width, height, title)) {
        return renderer;
    }
#endif
    return nullptr;
}


bool GDIRenderer::initialize(HWND hwnd) {
    this->hwnd = hwnd;
    
    // 获取窗口DC
    hdc = GetDC(hwnd);
    if (!hdc) {
        LOGE("获取DC失败");
        return false;
    }
    
    // 创建兼容DC用于双缓冲
    hdcMem = CreateCompatibleDC(hdc);
    if (!hdcMem) {
        LOGE("创建兼容DC失败");
        ReleaseDC(hwnd, hdc);
        return false;
    }
    
    // 获取客户区实际大小（不需要再次缩放，因为已经是实际像素大小）
    RECT rect;
    GetClientRect(hwnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    
    // 创建兼容位图
    hBitmap = CreateCompatibleBitmap(hdc, width, height);
    if (!hBitmap) {
        LOGE("创建兼容位图失败");
        DeleteDC(hdcMem);
        ReleaseDC(hwnd, hdc);
        return false;
    }
    
    SelectObject(hdcMem, hBitmap);
    
    // 清空背景
    clear(Color(255, 255, 255));
    present();
    
    // 启动渲染线程
    running = true;
    renderThread = std::thread([this]() {
        using namespace std::chrono;
        auto lastFrame = steady_clock::now();
        
        while (running) {
            auto now = steady_clock::now();
            auto frameTime = duration_cast<nanoseconds>(now - lastFrame);
            
            // 处理帧回调
            Choreographer::getInstance().doFrame(frameTime);
            
            // 渲染
            if (renderCallback) {
                renderCallback(this);
                present();
            }
            
            // 帧率控制
            lastFrame = now;
            std::this_thread::sleep_for(milliseconds(16));
        }
    });
    
    return true;
}

void GDIRenderer::setEventHandler(std::function<bool(const Event&)> handler) {
    eventHandler = std::move(handler);
}

int GDIRenderer::getWidth() const {
    return width;
}

int GDIRenderer::getHeight() const {
    return height;
}

void GDIRenderer::setVSync(bool enabled) {
    vsyncEnabled = enabled;
}

void GDIRenderer::setFrameRate(int fps) {
    targetFPS = fps;
}