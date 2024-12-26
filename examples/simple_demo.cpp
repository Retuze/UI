#include "core/debug.h"
#include "graphics/gdi_renderer.h"
#include "graphics/renderer.h"
#include "graphics/font_manager.h"
#include "widgets/text_view.h"
#include "widgets/button.h"
#include "layout/layout.h"
#include "widgets/linear_layout_view.h"
#include "activity/activity.h"
#include <memory>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
#include <sstream>

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
        Debug::Log("MainActivity::onCreate");
        
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
        Debug::Log("MainActivity::onResume");
    }
    
    void onPause() override {
        Debug::Log("MainActivity::onPause");
    }
    
    void onDestroy() override {
        Debug::Log("MainActivity::onDestroy");
    }
};

int main() {
    Debug::Initialize();
    
    try {
        Debug::Log("Creating renderer...");
        auto renderer = Renderer::create(800, 600, "Stopwatch");
        if (!renderer) {
            Debug::Error("Failed to create renderer");
            Debug::WaitForInput();
            return 1;
        }
        
        Debug::Log("Initializing font manager...");
        auto& fontManager = FontManager::getInstance();
        if (!fontManager.initialize()) {
            Debug::Error("Failed to initialize font manager");
            Debug::WaitForInput();
            return 1;
        }
        
        Debug::Log("Loading font...");
        if (!fontManager.loadFont("C:\\Windows\\Fonts\\segoe.ttf", 24)) {
            Debug::Log("Failed to load segoe.ttf, trying arial.ttf...");
            if (!fontManager.loadFont("C:\\Windows\\Fonts\\arial.ttf", 24)) {
                Debug::Error("Failed to load any font");
                Debug::WaitForInput();
                return 1;
            }
        }
        
        auto activity = std::make_unique<MainActivity>();
        activity->onCreate();
        activity->onResume();
        
        Debug::Log("Setting up event handler...");
        auto* gdiRenderer = static_cast<GDIRenderer*>(renderer.get());
        gdiRenderer->setEventHandler([&activity](const Event& event) {
            return activity->handleEvent(event);
        });
        
        Debug::Log("Entering main loop...");
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
        Debug::Error("Exception caught: ", e.what());
        Debug::WaitForInput();
        return 1;
    } catch (...) {
        Debug::Error("Unknown exception caught");
        Debug::WaitForInput();
        return 1;
    }
}