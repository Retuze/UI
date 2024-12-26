#pragma once
#include "widgets/widget.h"

class TextView : public Widget {
public:
    explicit TextView(const std::string& text);
    
    void draw(Renderer* renderer) override;
    bool handleEvent(const Event& event) override;
    
    void setText(const std::string& text);

private:
    std::string text;
}; 