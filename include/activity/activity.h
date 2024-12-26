#pragma once
#include "widgets/widget.h"
#include "layout/layout.h"

class Activity {
public:
    virtual ~Activity() = default;
    
    virtual void onCreate() = 0;
    virtual void onResume() = 0;
    virtual void onPause() = 0;
    virtual void onDestroy() = 0;
    
    void setContentView(Widget* root);
    Widget* getContentView() const { return rootView; }
    
    virtual bool handleEvent(const Event& event);
    virtual void draw(Renderer* renderer);

protected:
    Widget* rootView = nullptr;
    Layout* rootLayout = nullptr;
}; 