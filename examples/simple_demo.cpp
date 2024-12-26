#include "core/debug.h"
#include "graphics/gdi_renderer.h"
#include "graphics/renderer.h"
#include "graphics/font_manager.h"
#include "widgets/text_view.h"
#include "widgets/button.h"
#include "layout/layout.h"
#include <memory>
#include <chrono>
#include <thread>
#include <iostream>

int main() {
    Debug::Initialize();
    
    try {
        Debug::Log("Creating renderer...");
        auto renderer = Renderer::create(800, 600, "Widgets Test");
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
        
        Debug::Log("Creating widgets...");
        
        auto textView = new TextView("Click the button below!");
        textView->setBounds(Rect(300, 200, 200, 40));
        textView->setTextColor(Color(0, 0, 255));
        
        auto button = new Button("Click Me!");
        button->setBounds(Rect(340, 260, 120, 40));
        button->setOnClickListener([]() {
            Debug::Log("Button clicked!");
        });
        
        static_cast<GDIRenderer*>(renderer.get())->setEventHandler(
            [button](const Event& event) {
                return button->dispatchEvent(event);
            }
        );
        
        Debug::Log("Entering main loop...");
        while (renderer->processEvents()) {
            renderer->clear(Color(255, 255, 255));
            textView->draw(renderer.get());
            button->draw(renderer.get());
            renderer->present();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        
        delete textView;
        delete button;
        Debug::Log("Exiting normally");
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