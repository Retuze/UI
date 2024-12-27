#pragma once
#include "core/event.h"
#include "graphics/renderer.h"
#include <windows.h>
#include <vector>
#include <functional>

class GDIRenderer : public Renderer {
public:
    GDIRenderer(HWND hwnd = nullptr);
    ~GDIRenderer();
    bool createWindow(int width, int height, const char* title);
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    // 基础像素操作
    void setPixel(int x, int y, Color color) override;
    Color getPixel(int x, int y) const override;

    bool processEvents() override;
    int getWidth() const override { return width; }
    int getHeight() const override { return height; }

    
    // 内存块操作
    void blitRaw(int x, int y, const void* data, int width, int height, 
                 int stride, PixelFormat format) override;
    void copyRect(const Rect& src, const Rect& dst) override;
    
    // 基础绘图操作
    void drawLine(int x1, int y1, int x2, int y2, Color color) override;
    void fillRect(const Rect& rect, Color color) override;
    
    // 缓冲区操作
    void clear(Color color) override;
    void present(const Rect* dirtyRects = nullptr, int numRects = 0) override;
    
    // 剪切区域
    void pushClipRect(const Rect& rect) override;
    void popClipRect() override;
    
    void setEventHandler(std::function<bool(const Event&)> handler) {
        eventHandler = std::move(handler);
    }
    
    void setUpdateCallback(std::function<void()> callback) {
        updateCallback = std::move(callback);
    }
    
protected:
    void* lockRect(const Rect& rect, int* stride) override;
    void unlockRect() override;

private:
    HWND hwnd;
    HDC hdc;
    HDC hdcMem;
    HBITMAP hBitmap;
    int width;
    int height;
    std::vector<Rect> clipStack;
    void* lockedBits = nullptr;
    static ATOM registerClass();
    bool shouldClose = false;
    bool initialized = false;
    bool initialize(HWND hwnd);
    std::function<bool(const Event&)> eventHandler;
    bool sizeMoveTimerRunning = false;
    static const UINT_PTR SIZE_MOVE_TIMER_ID = 1;
    std::function<void()> updateCallback;
}; 