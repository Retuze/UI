#include "application/application.h"
#include "graphics/render_context.h"

class CustomView : public View {
    void onDraw(RenderContext& context) override {
        // 绘制一个红色矩形
        Paint paint;
        paint.setColor(Color(255, 0, 0)); // 红色
        
        Rect rect(100, 100, 200, 150);
        context.drawRect(rect, paint);
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
        }
    }
    
    app.onTerminate();
    return 0;
} 