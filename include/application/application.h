#pragma once
#include "activity/activity.h"
#include "graphics/render_context.h"
#include <vector>
#include <memory>

class Application {
public:
    static Application& getInstance();
    
    void onCreate();
    void onTerminate();
    void startActivity(Activity* activity);
    void finishActivity(Activity* activity);
    
    RenderContext* getRenderContext() { return renderContext.get(); }
    Activity* getCurrentActivity() { return currentActivity; }
    
private:
    std::vector<Activity*> activities;
    Activity* currentActivity = nullptr;
    std::unique_ptr<RenderContext> renderContext;
};
