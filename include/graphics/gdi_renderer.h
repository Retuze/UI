#pragma once
#include "core/event.h"
#include "graphics/renderer.h"
#include <windows.h>
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

class Activity;  // 前向声明

class GDIRenderer : public Renderer {
public:
    // 构造和初始化
    GDIRenderer(HWND hwnd);
    ~GDIRenderer();
    
    // 窗口管理
    bool createWindow(int width, int height, const char* title);
    bool isClosed() const { return shouldClose; }
    
    // 渲染循环控制
    bool processEvents() override;
    void setEventHandler(std::function<bool(const Event&)> handler);
    
    // 基础绘图操作
    void setPixel(int x, int y, Color color) override;
    Color getPixel(int x, int y) const override;
    void drawLine(int x1, int y1, int x2, int y2, Color color) override;
    void fillRect(const Rect& rect, Color color) override;
    
    // 缓冲区操作
    void clear(Color color) override;
    void present(const Rect* dirtyRects = nullptr, int numRects = 0) override;
    
    // 剪切区域
    void pushClipRect(const Rect& rect) override;
    void popClipRect() override;
    
    // 添加这个声明
    void copyRect(const Rect& src, const Rect& dst) override;
    void blitRaw(int x, int y, const void* data, int width, int height, 
                 int stride, PixelFormat format) override;
    
    // 添加缺失的纯虚函数声明
    int getWidth() const override;
    int getHeight() const override;
    
    void setVSync(bool enabled) override;
    void setFrameRate(int fps) override;
    bool isHardwareAccelerated() const override { return false; }
    
    // 添加渲染回调
    void setRenderCallback(std::function<void(Renderer*)> callback) {
        renderCallback = std::move(callback);
    }
    
    bool resizeBuffers(int newWidth, int newHeight);
    
protected:
    void* lockRect(const Rect& rect, int* stride) override;
    void unlockRect() override;
    
private:
    // 窗口相关
    HWND hwnd;
    HDC hdc;
    HDC hdcMem;
    HBITMAP hBitmap;
    int width;
    int height;
    bool shouldClose = false;
    
    // 渲染相关
    std::vector<Rect> clipStack;
    void* lockedBits = nullptr;
    std::thread renderThread;
    std::atomic<bool> running{false};
    
    // 回调函数
    std::function<bool(const Event&)> eventHandler;
    
    // 私有方法
    static ATOM registerClass();
    bool initialize(HWND hwnd);
    void renderLoop();
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    
    // 添加帧率控制相关
    std::mutex renderMutex;
    std::condition_variable renderCV;
    int targetFPS = 60;
    bool vsyncEnabled = false;
    
    std::function<void(Renderer*)> renderCallback;
}; 