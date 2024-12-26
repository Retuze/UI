#include "activity/activity.h"

void Activity::setContentView(Widget* root) {
    if (rootView) {
        delete rootView;
    }
    rootView = root;
}

bool Activity::handleEvent(const Event& event) {
    if (rootView) {
        return rootView->handleEvent(event);
    }
    return false;
}

void Activity::draw(Renderer* renderer) {
    if (rootView) {
        rootView->draw(renderer);
    }
} 