#include "platform/windows/window.h"

int main() {
    ui::windows::WindowsWindow window(L"UI Framework Demo", 800, 600);
    
    // 绘制一个渐变
    for (int y = 0; y < window.GetSize().height; ++y) {
        for (int x = 0; x < window.GetSize().width; ++x) {
            uint8_t intensity = static_cast<uint8_t>(x * 255 / window.GetSize().width);
            window.SetPixel(x, y, 0, 0, intensity);
        }
    }
    
    window.Show();
    window.Update();
    window.RunMessageLoop();
    
    return 0;
} 