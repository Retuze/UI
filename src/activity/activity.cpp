#include "activity/activity.h"

void Activity::setContentView(View* view) {
    if (contentView) {
        delete contentView;
    }
    contentView = view;
    
    // 如果Activity已经启动,需要触发布局
    if (contentView && isStarted()) {
        contentView->requestLayout();
        contentView->invalidate();
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
    delete contentView;
    contentView = nullptr;
} 