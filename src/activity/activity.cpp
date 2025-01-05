#include "activity/activity.h"
#include "application/application.h"
#include "view/window_manager.h"

Activity::Activity() {
    // Remove unnecessary rootLayout initialization
}

void Activity::setContentView(View* view) {
    setContentView(view, LayoutParams(LayoutParams::MATCH_PARENT, LayoutParams::MATCH_PARENT));
}

void Activity::setContentView(View* view, const LayoutParams& params) {
    if (contentView) {
        delete contentView;
    }
    contentView = view;
    if (view) {
        view->setLayoutParams(params);
        // Get the window manager and add the view
        if (auto* wm = Application::getInstance().getWindowManager()) {
            wm->addView(view, params);
        }
    }
}

bool Activity::isStarted() const {
    return state >= ActivityState::Started;
}

bool Activity::isResumed() const {
    return state == ActivityState::Resumed;
}

void Activity::dispatchCreate() {
    state = ActivityState::Created;
    onCreate();
}

void Activity::dispatchStart() {
    state = ActivityState::Started;
    onStart();
}

void Activity::dispatchResume() {
    state = ActivityState::Resumed;
    onResume();
}

void Activity::dispatchPause() {
    state = ActivityState::Paused;
    onPause();
}

void Activity::dispatchStop() {
    state = ActivityState::Stopped;
    onStop();
}

void Activity::dispatchDestroy() {
    state = ActivityState::Destroyed;
    onDestroy();
    if (contentView) {
        delete contentView;
        contentView = nullptr;
    }
}

WindowManager* Activity::getWindowManager() {
    return Application::getInstance().getWindowManager();
}

RenderContext* Activity::getRenderContext() {
    return Application::getInstance().getRenderContext();
}

Application* Activity::getApplication() {
    return &Application::getInstance();
}

std::string Activity::getResourcePath() const {
    return Application::getInstance().getResourcePath();
}

bool Activity::checkPermission(const std::string& permission) {
    return Application::getInstance().checkPermission(permission);
} 