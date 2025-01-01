#include "platform/win32/win32_surface.h"
#include "core/event.h"
#include "graphics/ui_thread.h"
#include "application/application.h"
#include "core/choreographer.h"
#include <stdexcept>
#include <windowsx.h>

namespace {
    constexpr const wchar_t* WINDOW_CLASS_NAME = L"SimpleGUISurface";
}

// Surface工厂方法实现
std::unique_ptr<Surface> Surface::create(int width, int height, PixelFormat format) {
    return std::make_unique<Win32Surface>(width, height, format);
}

Win32Surface::Win32Surface(int width, int height, PixelFormat format) 
    : width(width), height(height), format(format) {
}

Win32Surface::~Win32Surface() {
    if (memBitmap) {
        DeleteObject(memBitmap);
    }
    if (memDC) {
        DeleteDC(memDC);
    }
    if (hwnd) {
        DestroyWindow(hwnd);
    }
}

ATOM Win32Surface::registerClass() {
    static ATOM atom = 0;
    if (atom == 0) {
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Win32Surface::WndProc;
        wcex.hInstance = GetModuleHandle(NULL);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.lpszClassName = WINDOW_CLASS_NAME;
        
        atom = RegisterClassExW(&wcex);
        if (atom == 0) {
            throw std::runtime_error("Failed to register window class");
        }
    }
    return atom;
}

bool Win32Surface::initialize() {
    // 注册窗口类
    registerClass();
    
    // 创建窗口
    hwnd = CreateWindowW(
        WINDOW_CLASS_NAME,
        L"SimpleGUI",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        NULL, NULL,
        GetModuleHandle(NULL),
        this
    );
    
    if (!hwnd) {
        return false;
    }
    
    // 创建内存DC和位图
    HDC hdc = GetDC(hwnd);
    memDC = CreateCompatibleDC(hdc);
    ReleaseDC(hwnd, hdc);
    
    if (!memDC) {
        return false;
    }
    
    // 设置位图信息
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // 负值表示从上到下
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = width * height * 4;
    
    // 创建DIB
    memBitmap = CreateDIBSection(
        memDC,
        &bmi,
        DIB_RGB_COLORS,
        &bits,
        NULL,
        0
    );
    
    if (!memBitmap) {
        return false;
    }
    
    SelectObject(memDC, memBitmap);
    
    // 显示窗口
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    return true;
}

void* Win32Surface::lock(int* stride) {
    if (stride) {
        *stride = width * 4;  // 32位色深
    }
    return bits;
}

void Win32Surface::unlock() {
    // 在这个实现中不需要做什么
}

void Win32Surface::present() {
    if (!hwnd || !memDC) return;
    
    HDC hdc = GetDC(hwnd);
    if (hdc) {
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
        ReleaseDC(hwnd, hdc);
    }
}

LRESULT CALLBACK Win32Surface::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Win32Surface* surface = nullptr;
    
    if (msg == WM_CREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        surface = static_cast<Win32Surface*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(surface));
        return 0;
    } else {
        surface = reinterpret_cast<Win32Surface*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    if (surface) {
        Event event;
        bool hasEvent = false;
        
        switch (msg) {
            case WM_CLOSE:
                DestroyWindow(hwnd);
                return 0;
                
            case WM_DESTROY:
                if (surface) {
                    Event event;
                    event.type = EventType::Quit;
                    surface->queueEvent(event);
                }
                PostQuitMessage(0);
                return 0;
                
            case WM_MOUSEMOVE:
                event.type = EventType::MouseMove;
                event.x = GET_X_LPARAM(lParam);
                event.y = GET_Y_LPARAM(lParam);
                hasEvent = true;
                break;
                
            case WM_LBUTTONDOWN:
                event.type = EventType::MousePress;
                event.x = GET_X_LPARAM(lParam);
                event.y = GET_Y_LPARAM(lParam);
                event.button = 1;
                hasEvent = true;
                break;
                
            case WM_LBUTTONUP:
                event.type = EventType::MouseRelease;
                event.x = GET_X_LPARAM(lParam);
                event.y = GET_Y_LPARAM(lParam);
                event.button = 1;
                hasEvent = true;
                break;
        }
        
        if (hasEvent) {
            // 将事件加入队列,而不是直接调用UI线程
            surface->queueEvent(event);
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Win32Surface::resizeBuffers(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
    
    if (memBitmap) {
        DeleteObject(memBitmap);
    }
    
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    
    memBitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    SelectObject(memDC, memBitmap);
}

bool Win32Surface::pollEvent(Event& event) {
    // 先检查是否有 Windows 消息
    MSG msg;
    while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            event.type = EventType::Quit;
            return true;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // 检查事件队列
    std::lock_guard<std::mutex> lock(eventMutex);
    if (!eventQueue.empty()) {
        event = eventQueue.front();
        eventQueue.pop();
        return true;
    }
    
    return false;
} 