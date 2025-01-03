#include "activity/activity.h"
#include "application/application.h"

Activity::Activity() {
    rootLayout = std::make_unique<LinearLayout>(LinearLayout::Orientation::Vertical);
    rootLayout->setLayoutParams({LayoutParams::MATCH_PARENT, LayoutParams::MATCH_PARENT});
}

void Activity::setContentView(View* view) {
    setContentView(view, LayoutParams(LayoutParams::MATCH_PARENT, LayoutParams::MATCH_PARENT));
}

void Activity::setContentView(View* view, const LayoutParams& params) {
    if (contentView) {
        rootLayout->removeView(contentView);
        delete contentView;
    }
    contentView = view;
    if (view) {
        view->setLayoutParams(params);
        rootLayout->addView(view);
        viewRoot.setView(rootLayout.get());
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
    if (auto* context = Application::getInstance().getRenderContext()) {
        viewRoot.setRenderContext(context);
    }
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
    delete contentView;
    contentView = nullptr;
} 