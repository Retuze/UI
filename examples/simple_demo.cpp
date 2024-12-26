#include "graphics/renderer.h"
#include "graphics/font_manager.h"
#include "widgets/text_view.h"
#include <memory>
#include <chrono>
#include <thread>

int main() {
    auto renderer = Renderer::create(800, 600, "Widgets Test");
    if (!renderer) {
        return 1;
    }
    
    // 初始化字体管理器
    auto& fontManager = FontManager::getInstance();
    if (!fontManager.initialize()) {
        return 1;
    }
    if (!fontManager.loadFont("C:\\Windows\\Fonts\\msyh.ttf", 24)) {
        return 1;
    }
    
    // 创建一些测试控件
    auto textView1 = std::make_unique<TextView>("Hello, SimpleGUI!");
    textView1->setBounds(Rect(50, 50, 200, 40));
    textView1->setTextColor(Color(0, 0, 255));  // 蓝色
    
    auto textView2 = std::make_unique<TextView>("这是中文测试");
    textView2->setBounds(Rect(50, 100, 200, 40));
    textView2->setTextColor(Color(255, 0, 0));  // 红色
    textView2->setPadding(10, 5, 10, 5);
    
    // 主循环
    while (renderer->processEvents()) {
        renderer->clear(Color(255, 255, 255));
        
        textView1->draw(renderer.get());
        textView2->draw(renderer.get());
        
        renderer->present();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    return 0;
} 