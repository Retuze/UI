#include "application/application.h"
#include "graphics/render_context.h"
#include <thread>
#include <chrono>
#include "core/logger.h"
#include "graphics/font_manager.h"

LOG_TAG("demo");
class CustomView : public View {
    void onDraw(RenderContext& context) override {
        LOGI("CustomView::onDraw - Start drawing");
        
        // 1. 先绘制白色背景
        Paint bgPaint;
        bgPaint.setColor(Color(255, 255, 255));  // 白色
        context.drawRect(bounds, bgPaint);
        LOGI("CustomView::onDraw - Background drawn");
        
        // 2. 绘制红色矩形作为参考
        Paint paint;
        paint.setColor(Color(255, 0, 0)); // 红色
        Rect rect(100, 100, 200, 150);
        context.drawRect(rect, paint);
        LOGI("CustomView::onDraw - Red rectangle drawn at (%d,%d,%d,%d)", 
             rect.x, rect.y, rect.width, rect.height);
        
        // 3. 绘制文本，使用更大的字号和明显的颜色
        Paint textPaint;
        textPaint.setColor(Color(0, 0, 255));  // 蓝色
        textPaint.setTextSize(32.0f);  // 更大的字号
        
        // 在不同位置绘制文本进行测试
        LOGI("CustomView::onDraw - About to draw text with size=%.1f, color=(%d,%d,%d)", 
             textPaint.getTextSize(), 
             textPaint.getColor().r,
             textPaint.getColor().g,
             textPaint.getColor().b);
             
        context.drawText("Test 1", 50, 50, textPaint);
        LOGI("CustomView::onDraw - Drew 'Test 1' at (50,50)");
        
        context.drawText("Test 2", 100, 90, textPaint);
        LOGI("CustomView::onDraw - Drew 'Test 2' at (100,90)");
        
        context.drawText("Test 3", 100, 180, textPaint);
        LOGI("CustomView::onDraw - Drew 'Test 3' at (100,180)");
        
        context.drawText("你好", 200, 200, textPaint);
        LOGI("CustomView::onDraw - Drew '你好' at (200,200)");
        
        LOGI("CustomView::onDraw - Finished drawing");
    }
};

class TestScreen : public Activity {
public:
    void onCreate() override {
        auto* customView = new CustomView();
        customView->setLayoutParams({800, 600});
        setContentView(customView);
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
    if (!fontManager.loadFont("C:/Windows/Fonts/msyh.ttc", 32)) {  // Windows 下的微软雅黑字体
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
        
        if (isRunning) {
            app.render();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    
    app.onTerminate();
    return 0;
} 