#include "image_viewer.h"
#include <windowsx.h>

namespace {
    // 使用宏来确保正确的字符串类型
    #ifdef UNICODE
        const TCHAR* WINDOW_CLASS_NAME = L"ImageViewerClass";
        const TCHAR* WINDOW_PROP_NAME = L"ImageViewer_This";
    #else
        const TCHAR* WINDOW_CLASS_NAME = "ImageViewerClass";
        const TCHAR* WINDOW_PROP_NAME = "ImageViewer_This";
    #endif
}

bool ImageViewer::RegisterWindowClass() {
    static bool registered = false;
    if (registered) return true;

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = WINDOW_CLASS_NAME;

    registered = RegisterClassEx(&wc) != 0;
    return registered;
}

LRESULT CALLBACK ImageViewer::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    ImageViewer* viewer = reinterpret_cast<ImageViewer*>(
        GetPropW(hwnd, WINDOW_PROP_NAME));  // 明确使用Unicode版本

    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetPropW(hwnd, WINDOW_PROP_NAME, cs->lpCreateParams);  // 明确使用Unicode版本
        return 0;
    }

    case WM_PAINT: {
        if (!viewer) break;
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        BitBlt(hdc, 0, 0, viewer->m_width, viewer->m_height,
               viewer->m_memDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        RemovePropW(hwnd, WINDOW_PROP_NAME);  // 明确使用Unicode版本
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

ImageViewer::ImageViewer(const wchar_t* windowTitle, int width, int height) 
    : m_width(width), m_height(height), m_hwnd(nullptr), 
      m_memDC(nullptr), m_bitmap(nullptr), m_bits(nullptr) {
    
    RegisterWindowClass();
    
    RECT rc = {0, 0, width, height};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    
    m_hwnd = CreateWindowExW(  // 明确使用Unicode版本
        0, WINDOW_CLASS_NAME, windowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, GetModuleHandle(nullptr), this
    );

    if (!m_hwnd) return;

    HDC hdc = GetDC(m_hwnd);
    m_memDC = CreateCompatibleDC(hdc);
    
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    m_bitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, 
                               reinterpret_cast<void**>(&m_bits), nullptr, 0);
    SelectObject(m_memDC, m_bitmap);
    
    ReleaseDC(m_hwnd, hdc);
    ShowWindow(m_hwnd, SW_SHOW);
}

ImageViewer::~ImageViewer() {
    if (m_bitmap) DeleteObject(m_bitmap);
    if (m_memDC) DeleteDC(m_memDC);
    if (m_hwnd) DestroyWindow(m_hwnd);
}

void ImageViewer::ShowImage(const uint8_t* buffer, int width, int height) {
    if (!m_bits || width != m_width || height != m_height) return;
    
    memcpy(m_bits, buffer, width * height * 4);
    InvalidateRect(m_hwnd, nullptr, FALSE);
    UpdateWindow(m_hwnd);
}

bool ImageViewer::ProcessMessages() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) return false;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}  