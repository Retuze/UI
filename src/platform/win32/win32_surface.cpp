#include "platform/win32/win32_surface.h"
#include "platform/event_translator.h"
#include "core/event_dispatcher.h"
#include "core/logger.h"
#include <stdexcept>

LOG_TAG("Win32Surface");

Win32Surface::Win32Surface(const SurfaceConfig& config) : config(config) {
    // 加载DWM库
    dwmLib = LoadLibraryA("dwmapi.dll");
    if (dwmLib) {
        DwmFlush = reinterpret_cast<DwmFlushProc>(
            GetProcAddress(dwmLib, "DwmFlush"));
        DwmIsCompositionEnabled = reinterpret_cast<DwmIsCompositionEnabledProc>(
            GetProcAddress(dwmLib, "DwmIsCompositionEnabled"));
    }
    
    // 强制使用BGRA8888格式,这是GDI的标准格式
    this->config.format = PixelFormat::BGRA8888_LE();
    
    // 只创建缓冲区数组
    buffers.resize(config.bufferCount);
}

Win32Surface::~Win32Surface() {
    destroy();
    if (dwmLib) {
        FreeLibrary(dwmLib);
        dwmLib = nullptr;
    }
}

bool Win32Surface::initialize() {
    if (initialized) {
        return true;
    }

    // 启动窗口线程
    windowThread = std::thread(&Win32Surface::windowThreadProc, this);
    
    // 等待窗口创建完成
    std::unique_lock<std::mutex> lock(windowMutex);
    windowCreated.wait(lock, [this]() { return hwnd != nullptr; });
    
    // 初始化GDI缓冲区
    HDC hdc = GetDC(hwnd);
    for (auto& buffer : buffers) {
        buffer.dc = CreateCompatibleDC(hdc);
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = config.width;
        bmi.bmiHeader.biHeight = -config.height; // 负值表示从上到下
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = config.format.getBitsPerPixel();
        bmi.bmiHeader.biCompression = BI_RGB;
        
        void* bits = nullptr;
        buffer.winBitmap = CreateDIBSection(buffer.dc, &bmi, DIB_RGB_COLORS,
                                          &bits, NULL, 0);
        
        // 创建Bitmap并直接使用DIBSection的缓冲区
        buffer.bitmap = std::make_unique<Bitmap>(
            config.width,
            config.height,
            config.format
        );
        buffer.bitmap->setPixels(bits);  // 使用DIBSection的缓冲区
        SelectObject(buffer.dc, buffer.winBitmap);
    }
    ReleaseDC(hwnd, hdc);
    
    initialized = true;
    return true;
}

void Win32Surface::destroy() {
    if (hwnd) {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
    
    if (windowThread.joinable()) {
        windowThread.join();
    }
    
    // 清理缓冲区
    for (auto& buffer : buffers) {
        if (buffer.winBitmap) {
            DeleteObject(buffer.winBitmap);
        }
        if (buffer.dc) {
            DeleteDC(buffer.dc);
        }
        buffer.bitmap.reset();
    }
    buffers.clear();
    
    initialized = false;
}

Bitmap* Win32Surface::lockBuffer() {
    std::unique_lock<std::mutex> lock(bufferMutex);
    
    // 查找下一个可用缓冲区
    for (size_t i = 0; i < buffers.size(); i++) {
        int index = (currentBuffer + i + 1) % buffers.size();
        if (!buffers[index].inUse) {
            currentBuffer = index;
            buffers[index].inUse = true;
            return buffers[index].bitmap.get();
        }
    }
    
    // 等待缓冲区可用
    bufferAvailable.wait(lock);
    return buffers[currentBuffer].bitmap.get();
}

void Win32Surface::unlockBuffer() {
    std::unique_lock<std::mutex> lock(bufferMutex);
    displayQueue.push(currentBuffer);
}

void Win32Surface::present() {
    std::unique_lock<std::mutex> lock(bufferMutex);
    
    if (displayQueue.empty()) {
        return;
    }
    
    int displayIndex = displayQueue.front();
    displayQueue.pop();
    
    if (config.vsyncEnabled) {
        waitVSync();
    }
    
    // 直接使用BitBlt显示,因为我们知道格式是BGRA8888
    if (hwnd && buffers[displayIndex].dc) {
        HDC hdc = GetDC(hwnd);
        if (hdc) {
            BitBlt(hdc, 0, 0, config.width, config.height,
                   buffers[displayIndex].dc, 0, 0, SRCCOPY);
            ReleaseDC(hwnd, hdc);
        }
    }
    
    buffers[displayIndex].inUse = false;
    bufferAvailable.notify_one();
}

void Win32Surface::windowThreadProc() {
    // 注册窗口类
    registerClass();
    
    // 创建窗口
    RECT rect = {0, 0, config.width, config.height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    
    hwnd = CreateWindowW(L"SimpleGUISurface", L"SimpleGUI",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        rect.right - rect.left,
                        rect.bottom - rect.top,
                        nullptr, nullptr,
                        GetModuleHandle(NULL), this);
                        
    if (!hwnd) {
        LOGE("Failed to create window");
        return;
    }
    
    // 通知窗口创建完成
    {
        std::lock_guard<std::mutex> lock(windowMutex);
        windowCreated.notify_all();
    }
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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
        wcex.lpszClassName = L"SimpleGUISurface";
        
        atom = RegisterClassExW(&wcex);
        if (atom == 0) {
            throw std::runtime_error("Failed to register window class");
        }
    }
    return atom;
}

LRESULT CALLBACK Win32Surface::WndProc(HWND hwnd, UINT msg,
                                      WPARAM wParam, LPARAM lParam) {
    Win32Surface* surface = nullptr;
    
    if (msg == WM_CREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        surface = static_cast<Win32Surface*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA,
                        reinterpret_cast<LONG_PTR>(surface));
    } else {
        surface = reinterpret_cast<Win32Surface*>(
            GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    // 处理窗口消息并转换为标准事件
    if (surface) {
        Event event;
        static Win32EventTranslator translator;
        
        switch (msg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
                
            case WM_SIZE: 
                if (wParam != SIZE_MINIMIZED) {
                    int width = LOWORD(lParam);
                    int height = HIWORD(lParam);
                    surface->resizeBuffers(width, height);
                }
                
            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_MOUSEWHEEL:
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_CHAR:
            case WM_SETFOCUS:
            case WM_KILLFOCUS: {
                // 统一的事件转换和分发逻辑
                MSG winMsg = {hwnd, msg, wParam, lParam};
                if (translator.translateNativeEvent(&winMsg, event)) {
                    EventDispatcher::getInstance().dispatchEvent(event);
                }
                break;
            }
        }
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Win32Surface::waitVSync() {
    // 使用DWM合成器的VSync同步
    if (DwmIsCompositionEnabled && DwmFlush) {
        BOOL enabled = FALSE;
        if (SUCCEEDED(DwmIsCompositionEnabled(&enabled)) && enabled) {
            DwmFlush();
            return;
        }
    }
    
    // 如果DWM不可用，使用固定帧率同步
    static const int64_t frameInterval = 16666667; // 60fps in nanoseconds
    static int64_t lastVSync = 0;
    
    int64_t now = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
    
    int64_t nextVSync = lastVSync + frameInterval;
    if (nextVSync > now) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(nextVSync - now));
    }
    
    lastVSync = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

void Win32Surface::setVSyncEnabled(bool enabled) {
    config.vsyncEnabled = enabled;
}

void Win32Surface::resizeBuffers(int width, int height) {
    std::lock_guard<std::mutex> lock(resizeMutex);
    std::lock_guard<std::mutex> bufferLock(bufferMutex);

    // 更新配置
    config.width = width;
    config.height = height;

    // 获取DC
    HDC hdc = GetDC(hwnd);

    // 清理旧缓冲区
    for (auto& buffer : buffers) {
        if (buffer.winBitmap) {
            DeleteObject(buffer.winBitmap);
        }
        if (buffer.dc) {
            DeleteDC(buffer.dc);
        }
        buffer.bitmap.reset();
    }

    // 创建新缓冲区
    for (auto& buffer : buffers) {
        buffer.dc = CreateCompatibleDC(hdc);
        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height; // 负值表示从上到下
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = config.format.getBitsPerPixel();
        bmi.bmiHeader.biCompression = BI_RGB;
        
        void* bits = nullptr;
        buffer.winBitmap = CreateDIBSection(buffer.dc, &bmi, DIB_RGB_COLORS,
                                          &bits, NULL, 0);
        
        buffer.bitmap = std::make_unique<Bitmap>(
            width,
            height,
            config.format
        );
        buffer.bitmap->setPixels(bits);
        SelectObject(buffer.dc, buffer.winBitmap);
        buffer.inUse = false;
    }

    ReleaseDC(hwnd, hdc);
    
    // 清空显示队列
    std::queue<int> empty;
    std::swap(displayQueue, empty);
    
    // 重置当前缓冲区索引
    currentBuffer = 0;
    
    // 通知等待的线程
    bufferAvailable.notify_all();
}