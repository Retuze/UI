#include "activity/activity.h"
#include "application/application.h"
#include "view/window_manager.h"
#include "core/logger.h"

LOG_TAG("Activity");

Activity::Activity() : state(ActivityState::Created) {}

void Activity::setContentView(View* view) {
    if (contentView) {
        delete contentView;
    }
    contentView = view;
}

void Activity::setContentView(View* view, const LayoutParams& params) {
    setContentView(view);
    if (contentView) {
        contentView->setLayoutParams(params);
    }
}

void Activity::finish() {
    if (Application* app = getApplication()) {
        app->finishActivity(this);
    }
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
}

void Activity::dispatchSaveInstanceState() {
    if (!isStateSaved) {
        onSaveInstanceState();
        isStateSaved = true;
    }
}

void Activity::dispatchRestoreInstanceState() {
    if (isStateSaved) {
        onRestoreInstanceState();
        isStateSaved = false;
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