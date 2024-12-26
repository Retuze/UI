#include "graphics/renderer.h"
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
    
    // 主循环
    while (renderer->processEvents()) {  // 处理平台相关的事件
        // 清屏为白色
        renderer->clear(Color(255, 255, 255));
        
        // 测试基本图形绘制
        renderer->fillRect(Rect(50, 50, 100, 100), Color(255, 0, 0));    // 红色矩形
        renderer->drawLine(200, 50, 300, 150, Color(0, 255, 0));         // 绿色线条
        
        // 测试剪切区域
        renderer->pushClipRect(Rect(250, 200, 100, 100));
        renderer->fillRect(Rect(200, 150, 200, 200), Color(0, 0, 255));  // 蓝色矩形(部分被裁剪)
        renderer->popClipRect();
        
        renderer->present();  // 确保调用 present 显示到屏幕
        
        // 简单的帧率控制
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60fps
    }
    
    return 0;
} 