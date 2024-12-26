#pragma once
#include "widgets/widget.h"

class Button : public Widget {
public:
    explicit Button(const std::string& text);
    
    void draw(Renderer* renderer) override;
    bool handleEvent(const Event& event) override;
    
    void setText(const std::string& text);
    void setOnClickListener(std::function<void()> listener);

private:
    std::string text;
    std::function<void()> onClick;
    bool pressed;
}; 