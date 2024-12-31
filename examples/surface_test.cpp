#include "application/application.h"
#include "graphics/render_context.h"
#include "graphics/paint.h"
#include "graphics/font_manager.h"
#include "graphics/ui_thread.h"
#include "core/looper.h"
#include "core/choreographer.h"
#include <thread>

int main() {
    // 初始化UI线程
    auto& uiThread = UIThread::getInstance();
    uiThread.initialize();
    
    // 初始化应用
    auto& app = Application::getInstance();
    app.onCreate();
    
    // 初始化字体
    uiThread.runOnUiThread([]() {
        auto& fontManager = FontManager::getInstance();
        fontManager.initialize();
        fontManager.loadFont("C:/Windows/Fonts/msyh.ttc");
    });
    
    // 主消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_QUIT) {
            break;
        }
        
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        // 触发 Choreographer 回调
        uiThread.runOnUiThread([]() {
            auto& choreographer = Choreographer::getInstance();
            auto frameTime = std::chrono::steady_clock::now().time_since_epoch();
            choreographer.doFrame(frameTime);
        });
    }
    
    // 确保UI线程正确退出
    uiThread.quit();
    
    // 清理应用资源
    app.onTerminate();
    return 0;
} 