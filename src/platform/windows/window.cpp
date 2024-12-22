#include "platform/windows/window.h"
#include <stdexcept>

namespace ui {
namespace windows {

WindowsWindow::WindowsWindow(const std::wstring& title, int width, int height)
    : m_hwnd(nullptr)
    , m_memDC(nullptr)
    , m_bitmap(nullptr)
    , m_shouldClose(false)
    , m_width(width)
    , m_height(height)
    , m_buffer(width * height * 4)
    , m_title(title)
{
    static const wchar_t CLASS_NAME[] = L"UIFrameworkWindow";
    
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    if (!GetClassInfoExW(GetModuleHandle(nullptr), CLASS_NAME, &wc)) {
        if (!RegisterClassExW(&wc)) {
            throw std::runtime_error("Failed to register window class");
        }
    }

    RECT rect = { 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        this
    );

    if (!m_hwnd) {
        throw std::runtime_error("Failed to create window");
    }

    CreateBackBuffer();
    Clear();
    CenterWindow();
}

WindowsWindow::~WindowsWindow() {
    if (m_bitmap) DeleteObject(m_bitmap);
    if (m_memDC) DeleteDC(m_memDC);
    if (m_hwnd) DestroyWindow(m_hwnd);
}

void WindowsWindow::Show() {
    ShowWindow(m_hwnd, SW_SHOW);
}

void WindowsWindow::Hide() {
    ShowWindow(m_hwnd, SW_HIDE);
}

void WindowsWindow::Close() {
    m_shouldClose = true;
}

void WindowsWindow::Update() {
    PresentBuffer();
}

void WindowsWindow::SetPixel(int x, int y, uint8_t b, uint8_t g, uint8_t r, uint8_t a) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return;

    int index = (y * m_width + x) * 4;
    m_buffer[index + 0] = b;
    m_buffer[index + 1] = g;
    m_buffer[index + 2] = r;
    m_buffer[index + 3] = a;
}

void WindowsWindow::Clear(uint8_t b, uint8_t g, uint8_t r, uint8_t a) {
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            SetPixel(x, y, b, g, r, a);
        }
    }
}

Size WindowsWindow::GetSize() const {
    return Size(m_width, m_height);
}

void WindowsWindow::SetSize(const Size& size) {
    // TODO: Implement window resizing
}

Point WindowsWindow::GetPosition() const {
    RECT rect;
    GetWindowRect(m_hwnd, &rect);
    return Point(rect.left, rect.top);
}

void WindowsWindow::SetPosition(const Point& pos) {
    SetWindowPos(m_hwnd, nullptr, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

std::wstring WindowsWindow::GetTitle() const {
    return m_title;
}

void WindowsWindow::SetTitle(const std::wstring& title) {
    m_title = title;
    SetWindowTextW(m_hwnd, title.c_str());
}

void WindowsWindow::RunMessageLoop() {
    MSG msg = {};
    while (!m_shouldClose) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

bool WindowsWindow::IsRunning() const {
    return !m_shouldClose;
}

LRESULT CALLBACK WindowsWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WindowsWindow* window = nullptr;

    if (uMsg == WM_CREATE) {
        CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<WindowsWindow*>(createStruct->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window) {
        switch (uMsg) {
            case WM_CLOSE:
                window->m_shouldClose = true;
                return 0;

            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                window->PresentBuffer();
                EndPaint(hwnd, &ps);
                return 0;
            }
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void WindowsWindow::CreateBackBuffer() {
    HDC hdc = GetDC(m_hwnd);
    m_memDC = CreateCompatibleDC(hdc);
    
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_width;
    bmi.bmiHeader.biHeight = -m_height; // 上下方向
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits;
    m_bitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    SelectObject(m_memDC, m_bitmap);
    
    ReleaseDC(m_hwnd, hdc);
}

void WindowsWindow::PresentBuffer() {
    HDC hdc = GetDC(m_hwnd);
    
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_width;
    bmi.bmiHeader.biHeight = -m_height;  // 注意这里改为负值，保持正确的方向
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SetDIBits(
        hdc,
        m_bitmap,
        0,
        m_height,
        m_buffer.data(),
        &bmi,
        DIB_RGB_COLORS
    );

    BitBlt(
        hdc,
        0, 0,
        m_width,
        m_height,
        m_memDC,
        0, 0,
        SRCCOPY
    );

    ReleaseDC(m_hwnd, hdc);
}

void WindowsWindow::CenterWindow() {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    RECT rect;
    GetWindowRect(m_hwnd, &rect);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    SetWindowPos(m_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

} // namespace windows
} // namespace ui 