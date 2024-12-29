#include "activity/activity.h"
#include "core/logger.h"
#include "graphics/dpi_manager.h"
#include "graphics/font_manager.h"
#include "graphics/gdi_renderer.h"
#include "graphics/renderer.h"
#include "layout/layout.h"
#include "widgets/button.h"
#include "widgets/linear_layout_view.h"
#include "widgets/text_view.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>
#include "core/choreographer.h"


LOG_TAG("SimpleDemo");

class MainActivity : public Activity
{
private:
  TextView* timeDisplay;
  Button* startStopButton;
  Button* resetButton;
  bool running = false;
  std::chrono::steady_clock::time_point startTime;
  std::chrono::milliseconds elapsedTime{0};

  void updateTimer() {
    if (running) {
      auto now = std::chrono::steady_clock::now();
      elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - startTime);
      
      // 格式化时间
      std::stringstream ss;
      ss << std::setfill('0') << std::setw(2)
         << (elapsedTime.count() / 60000) << ":"
         << std::setfill('0') << std::setw(2)
         << ((elapsedTime.count() / 1000) % 60) << "."
         << std::setfill('0') << std::setw(3)
         << (elapsedTime.count() % 1000);
      
      timeDisplay->setText(ss.str());
    }
  }

  void startTimer() {
    running = true;
    startTime = std::chrono::steady_clock::now() - elapsedTime;
    
    // 注册帧回调
    Choreographer::getInstance().postFrameCallback([this](auto frameTime) {
        if (running) {
            auto now = std::chrono::steady_clock::now();
            elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - startTime);
            
            updateDisplay();
        }
    });
  }

  void updateDisplay() {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2)
       << (elapsedTime.count() / 60000) << ":"
       << std::setfill('0') << std::setw(2)
       << ((elapsedTime.count() / 1000) % 60) << "."
       << std::setfill('0') << std::setw(3)
       << (elapsedTime.count() % 1000);
    
    timeDisplay->setText(ss.str());
  }

public:
  void onCreate() override
  {

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
        startTimer();  // 调用 startTimer 而不是直接设置状态
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
    });

    layout->addView(timeDisplay);
    layout->addView(startStopButton);
    layout->addView(resetButton);

    setContentView(layout);

    if (auto* root = dynamic_cast<ViewGroup*>(getContentView())) {
        root->postRepeating([this]() {
            if (running) {
                updateTimer();
            }
        }, std::chrono::milliseconds(16), this);  // 使用 this 作为 token
    }
  }

  void onResume() override
  {
    LOGI("MainActivity::onResume");
  }

  void onPause() override
  {
    LOGI("MainActivity::onPause");
  }

  void onDestroy() override
  {
    LOGI("MainActivity::onDestroy");
  }
};

int main()
{
  auto& logger = Logger::getInstance();
  logger.initialize();
  logger.setLevel(LogLevel::Debug);

  try {
    LOGI("Initializing DPI manager...");
    DPIManager::getInstance().initialize();

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

    // 设置渲染回调
    gdiRenderer->setRenderCallback([&activity](Renderer* renderer) {
      // 处理消息队列
      if (auto* root = dynamic_cast<ViewGroup*>(activity->getContentView())) {
        root->processMessages();
      }
      
      // 渲染
      activity->draw(renderer);
    });

    LOGI("Entering main loop...");
    while (!gdiRenderer->isClosed()) {
      // 处理Windows消息
      if (!gdiRenderer->processEvents()) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
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