#pragma once
#include <windows.h>
#include <cstdint>

class ImageViewer {
public:
    ImageViewer(const wchar_t* windowTitle, int width, int height);
    ~ImageViewer();

    void ShowImage(const uint8_t* buffer, int width, int height);
    bool ProcessMessages();

private:
    static bool RegisterWindowClass();
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd;
    HDC m_memDC;
    HBITMAP m_bitmap;
    uint8_t* m_bits;
    int m_width;
    int m_height;
}; 