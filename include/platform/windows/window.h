#pragma once
#include "platform/interface/window.h"
#include <windows.h>
#include <vector>
#include "font/freetype_font.h"

namespace ui {
namespace windows {

class WindowsWindow : public IWindow {
public:
    WindowsWindow(const std::wstring& title, int width, int height);
    ~WindowsWindow() override;

    // 禁止拷贝
    WindowsWindow(const WindowsWindow&) = delete;
    WindowsWindow& operator=(const WindowsWindow&) = delete;

    // IWindow接口实现
    void Show() override;
    void Hide() override;
    void Close() override;
    void Update() override;
    
    void SetPixel(int x, int y, uint8_t b, uint8_t g, uint8_t r, uint8_t a = 255) override;
    void Clear(uint8_t b = 0, uint8_t g = 0, uint8_t r = 0, uint8_t a = 255) override;
    
    Size GetSize() const override;
    void SetSize(const Size& size) override;
    Point GetPosition() const override;
    void SetPosition(const Point& pos) override;
    std::wstring GetTitle() const override;
    void SetTitle(const std::wstring& title) override;
    
    void RunMessageLoop() override;
    bool IsRunning() const override;

    // 字体支持
    FontPtr CreateFontObject() override;
    void RenderText(const std::wstring& text, int x, int y, 
                   const Color& color, FontPtr font) override;

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void CreateBackBuffer();
    void PresentBuffer();
    void CenterWindow();

private:
    HWND m_hwnd;
    HDC m_memDC;
    HBITMAP m_bitmap;
    bool m_shouldClose;
    
    int m_width;
    int m_height;
    std::vector<uint8_t> m_buffer;
    std::wstring m_title;
}; 

} // namespace windows
} // namespace ui 