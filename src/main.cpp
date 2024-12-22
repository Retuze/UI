#include "platform/windows/window.h"

int main() {
    ui::windows::WindowsWindow window(L"UI Framework Demo", 800, 600);
    
    // 创建字体
    auto font = window.CreateFontObject();
    font->LoadFromFile(L"C:/Windows/Fonts/msyh.ttc", 24);
    
    // 清除背景
    window.Clear(255, 255, 255);
    
    // 渲染文本
    window.RenderText(L"Hello, 世界!", 100, 100, 
                     ui::Color(0, 0, 0), font);
    
    window.Show();
    window.Update();
    window.RunMessageLoop();
    
    return 0;
} 