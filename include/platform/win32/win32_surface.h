#pragma once
#include "graphics/surface.h"
#include <condition_variable>
#include <thread>
#include <queue>
#include <mutex>
#include <windows.h>
#include <memory>
#include <windowsx.h>

class Win32Surface : public Surface {
public:
    Win32Surface(const SurfaceConfig& config);
    ~Win32Surface() override;
    
    // Surface接口实现
    bool initialize() override;
    void destroy() override;
    
    // 缓冲区操作
    Bitmap* lockBuffer() override;
    void unlockBuffer() override;
    void present() override;
    
    // 显示控制
    void waitVSync() override;
    void setVSyncEnabled(bool enabled) override;
    
    // 属性访问
    int getWidth() const override { return config.width; }
    int getHeight() const override { return config.height; }
    PixelFormat getPixelFormat() const override { return config.format; }
    int getBufferCount() const override { return config.bufferCount; }
    bool isVSyncEnabled() const override { return config.vsyncEnabled; }
    
    // 事件处理
    bool pollEvent(Event& event) override;
    
    // Win32特有方法
    HWND getHWND() const { return hwnd; }
    
private:
    SurfaceConfig config;
    bool initialized = false;
    std::thread windowThread;
    
    // 窗口相关
    HWND hwnd = nullptr;
    std::mutex windowMutex;
    std::condition_variable windowCreated;
    
    // GDI缓冲区
    struct Buffer {
        std::unique_ptr<Bitmap> bitmap;
        HDC dc = nullptr;
        HBITMAP winBitmap = nullptr;
        bool inUse = false;
    };
    std::vector<Buffer> buffers;
    int currentBuffer = 0;
    std::queue<int> displayQueue;  // 显示队列
    
    // 同步相关
    std::mutex bufferMutex;
    std::condition_variable bufferAvailable;
    
    HMODULE dwmLib = nullptr;
    typedef HRESULT (WINAPI *DwmFlushProc)();
    typedef HRESULT (WINAPI *DwmIsCompositionEnabledProc)(BOOL*);
    
    DwmFlushProc DwmFlush = nullptr;
    DwmIsCompositionEnabledProc DwmIsCompositionEnabled = nullptr;
    
    static ATOM registerClass();
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void windowThreadProc();
}; 