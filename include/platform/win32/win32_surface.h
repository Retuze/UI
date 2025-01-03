#pragma once
#include "graphics/surface.h"
#include <queue>
#include <windows.h>

class Win32Surface : public Surface {
public:
    Win32Surface(int width, int height, PixelFormat format = PixelFormat::BGRA8888LE);
    ~Win32Surface() override;
    
    // Surface接口实现
    bool initialize() override;
    void* lock(int* stride) override;
    void unlock() override;
    void present() override;
    bool pollEvent(Event& event) override;
    bool close() override;
    
    int getWidth() const override { return width; }
    int getHeight() const override { return height; }
    PixelFormat getPixelFormat() const override { return format; }
    
    // Win32特有方法
    HWND getHWND() const { return hwnd; }
    
private:
    int width;
    int height;
    PixelFormat format;
    
    // GDI相关
    HWND hwnd = nullptr;
    HDC memDC = nullptr;
    HBITMAP memBitmap = nullptr;
    void* bits = nullptr;
    BITMAPINFO bmi = {};
    
    static ATOM registerClass();
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void resizeBuffers(int newWidth, int newHeight);
    std::queue<Event> eventQueue;
    std::mutex eventMutex;
    
    void queueEvent(const Event& event) {
        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push(event);
    }
}; 