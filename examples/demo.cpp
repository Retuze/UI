#include "application/application.h"
#include "activity/activity.h"
#include "widgets/text_view.h"
#include "widgets/button.h"
#include "view/linear_layout.h"
#include "graphics/font_manager.h"
#include "core/logger.h"
#include <thread>
#include <chrono>

LOG_TAG("demo");

class TestScreen : public Activity {
public:
    void onCreate() override {
        LOGI("TestScreen::onCreate");
        
        // 创建一个垂直布局容器
        auto* container = new LinearLayout(LinearLayout::Orientation::Vertical);
        container->setLayoutParams({
            LayoutParams::MATCH_PARENT,
            LayoutParams::MATCH_PARENT
        });
        container->setPadding(16, 16, 16, 16);
        
        // 创建TextView
        auto* textView = new TextView("Hello, World!");
        textView->setLayoutParams(
            LayoutParams(LayoutParams::WRAP_CONTENT, LayoutParams::WRAP_CONTENT)
                .setMargins(0, 16, 0, 0)
        );
        textView->setTextSize(24.0f);
        textView->setTextColor(Color(0, 0, 255));
        textView->setTextAlignment(TextAlignment::Center);
        
        // 创建Button
        auto* button = new Button("Click Me!");
        button->setLayoutParams(
            LayoutParams(LayoutParams::WRAP_CONTENT, LayoutParams::WRAP_CONTENT)
                .setMargins(0, 16, 0, 0)
        );
        button->setTextSize(20.0f);
        button->setOnClickListener([textView]() {
            textView->setText("Button Clicked!");
        });
        
        container->addView(textView);
        container->addView(button);
        
        setContentView(container);
    }
};

int main() {
    auto& app = Application::getInstance();
    app.onCreate();

    // 初始化字体管理器
    auto& fontManager = FontManager::getInstance();
    if (!fontManager.initialize()) {
        LOGE("Failed to initialize FontManager");
        return -1;
    }
    
    // 加载字体文件
    if (!fontManager.loadFont("C:/Windows/Fonts/msyh.ttc", 32)) {
        LOGE("Failed to load font");
        return -1;
    }
    
    app.startActivity(new TestScreen());
    
    bool isRunning = true;
    while (isRunning) {
        Event event;
        while (app.pollEvent(event)) {
            if (event.type == EventType::Quit) {
                isRunning = false;
                break;
            }
            app.dispatchEvent(event);
        }
        
        // 让出CPU时间片，但不要睡太久
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    app.onTerminate();
    return 0;
} 