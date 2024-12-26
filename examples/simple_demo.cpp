#include "graphics/renderer.h"
#include "graphics/font_manager.h"
#include <memory>
#include <chrono>
#include <thread>
#include <iostream>

int main() {
    // 创建渲染器（平台无关的接口）
    auto renderer = Renderer::create(800, 600, "Graphics Demo");
    if (!renderer) {
        std::cerr << "Failed to create renderer" << std::endl;
        return 1;
    }
    
    // 初始化字体管理器
    auto& fontManager = FontManager::getInstance();
    if (!fontManager.initialize()) {
        std::cerr << "Failed to initialize font manager" << std::endl;
        return 1;
    }
    
    // 加载字体文件 - 使用支持中文的字体
    if (!fontManager.loadFont("C:\\Windows\\Fonts\\msyh.ttf", 24)) {  // 微软雅黑
        // 如果找不到微软雅黑，尝试其他支持中文的字体
        if (!fontManager.loadFont("C:\\Windows\\Fonts\\simsun.ttc", 24)) {  // 宋体
            std::cerr << "Failed to load font" << std::endl;
            return 1;
        }
    }
    
    // 主循环
    while (renderer->processEvents()) {  // 处理平台相关的事件
        // 清屏为白色
        renderer->clear(Color(255, 255, 255));
        
        // 测试基本图形绘制
        renderer->fillRect(Rect(50, 50, 100, 100), Color(255, 0, 0));    // 红色矩形
        renderer->drawLine(200, 50, 300, 150, Color(0, 255, 0));         // 绿色线条
        
        // 测试文本渲染
        fontManager.renderText(renderer.get(), "Hello, FreeType!", 50, 200, Color(0, 0, 0));
        fontManager.renderText(renderer.get(), "这是中文测试", 50, 250, Color(255, 0, 0));
        
        renderer->present();  // 确保调用 present 显示到屏幕
        
        // 简单的帧率控制
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60fps
    }
    
    return 0;
} 