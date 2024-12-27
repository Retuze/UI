#include "core/logger.h"
#include "graphics/gdi_renderer.h"
#include "graphics/renderer.h"
#include "graphics/font_manager.h"
#include "widgets/text_view.h"
#include "widgets/button.h"
#include "layout/layout.h"
#include "widgets/linear_layout_view.h"
#include "activity/activity.h"
#include "graphics/dpi_manager.h"
#include <memory>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
#include <sstream>

LOG_TAG("SimpleDemo");

class MainActivity : public Activity {
private:
    TextView* timeDisplay;
    Button* startStopButton;
    Button* resetButton;
    bool running = false;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::milliseconds elapsedTime{0};
    std::chrono::steady_clock::time_point lastUpdateTime;
    static const int MIN_UPDATE_INTERVAL_MS = 16;

    void updateDisplay() {
        auto totalMillis = elapsedTime.count();
        int minutes = totalMillis / 60000;
        int seconds = (totalMillis % 60000) / 1000;
        int millis = totalMillis % 1000;
        
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << minutes << ":"
           << std::setfill('0') << std::setw(2) << seconds << "."
           << std::setfill('0') << std::setw(3) << millis;
        timeDisplay->setText(ss.str());
    }

public:
    void onCreate() override {
        
        auto layout = new LinearLayoutView(LinearLayout::Orientation::Vertical);
        layout->setBounds(Rect(0, 0, 800, 600));
        layout->setAlignment(LinearLayout::Alignment::Center);
        layout->setSpacing(20);
        
        timeDisplay = new TextView("00:00.000");
        timeDisplay->setTextColor(Color(0, 0, 0));
        timeDisplay->setSize(200, 40);
        
        startStopButton = new Button("Start");
        startStopButton->setSize(120, 40);
        startStopButton->setOnClickListener([this]() {
            LOGI("StartStopButton::onClick");
            if (!running) {
                running = true;
                startTime = std::chrono::steady_clock::now() - elapsedTime;
                startStopButton->setText("Stop");
            } else {
                running = false;
                elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - startTime);
                startStopButton->setText("Start");
            }
        });
        
        resetButton = new Button("Reset");
        resetButton->setSize(120, 40);
        resetButton->setOnClickListener([this]() {
            running = false;
            elapsedTime = std::chrono::milliseconds(0);
            startStopButton->setText("Start");
            updateDisplay();
        });
        
        layout->addView(timeDisplay);
        layout->addView(startStopButton);
        layout->addView(resetButton);
        
        setContentView(layout);
    }

    void onUpdate() override {
        if (running) {
            auto now = std::chrono::steady_clock::now();
            auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - lastUpdateTime).count();
            
            if (timeSinceLastUpdate >= MIN_UPDATE_INTERVAL_MS) {
                elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - startTime);
                updateDisplay();
                lastUpdateTime = now;
            }
        }
    }

    void onResume() override {
        LOGI("MainActivity::onResume");
    }
    
    void onPause() override {
        LOGI("MainActivity::onPause");
    }
    
    void onDestroy() override {
        LOGI("MainActivity::onDestroy");
    }
};

int main() {
    auto& logger = Logger::getInstance();
    logger.initialize();
    logger.setLevel(LogLevel::Debug);
    
    try {
        LOGI("Creating renderer...");
        auto renderer = Renderer::create(800, 600, "Stopwatch");
        if (!renderer) {
            LOGE("Failed to create renderer");
            logger.waitForInput();
            return 1;
        }
        
        LOGI("Initializing font manager...");
        auto& fontManager = FontManager::getInstance();
        if (!fontManager.initialize()) {
            LOGE("Failed to initialize font manager");
            logger.waitForInput();
            return 1;
        }
        
        LOGI("Loading font...");
        DPIManager::getInstance().initialize();
        Logger::getInstance().setLevel(LogLevel::Debug);
        if (!fontManager.loadFont("C:\\Windows\\Fonts\\arial.ttf", 24)) {
            LOGE("Failed to load font");
            return 1;
        }
        
        auto activity = std::make_unique<MainActivity>();
        activity->onCreate();
        activity->onResume();
        
        LOGI("Setting up event handler...");
        auto* gdiRenderer = static_cast<GDIRenderer*>(renderer.get());
        gdiRenderer->setEventHandler([&activity](const Event& event) {
            return activity->handleEvent(event);
        });
        
        gdiRenderer->setUpdateCallback([&]() {
            activity->onUpdate();
            activity->draw(gdiRenderer);
        });
        
        LOGI("Entering main loop...");
        while (renderer->processEvents()) {
            renderer->clear(Color(255, 255, 255));
            activity->onUpdate();
            activity->draw(renderer.get());
            renderer->present();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        
        activity->onPause();
        activity->onDestroy();
        return 0;
        
    } catch (const std::exception& e) {
        LOGE("Exception caught: ", e.what());
        logger.waitForInput();
        return 1;
    } catch (...) {
        LOGE("Unknown exception caught");
        logger.waitForInput();
        return 1;
    }
}