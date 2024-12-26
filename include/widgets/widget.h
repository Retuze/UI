#pragma once
#include "core/types.h"
#include "core/event.h"
#include "graphics/renderer.h"

class Widget {
public:
    Widget();
    virtual ~Widget() = default;
    
    virtual void draw(Renderer* renderer) = 0;
    virtual bool handleEvent(const Event& event) = 0;
    
    void setPosition(int x, int y);
    void setSize(int width, int height);
    void setBounds(const Rect& bounds);
    
    const Rect& getBounds() const { return bounds; }
    Widget* getParent() const { return parent; }
    
    void addChild(Widget* child);
    void removeChild(Widget* child);

protected:
    Rect bounds;
    Widget* parent;
    std::vector<Widget*> children;
    bool visible;
}; 