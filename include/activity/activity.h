#pragma once
#include "view/view.h"

class Activity {
public:
    enum class ActivityState {
        Created,
        Started,
        Resumed,
        Paused,
        Stopped,
        Destroyed
    };

    virtual ~Activity() = default;
    
    // 生命周期方法
    virtual void onCreate() = 0;
    virtual void onStart() = 0;
    virtual void onResume() = 0;
    virtual void onPause() = 0;
    virtual void onStop() = 0;
    virtual void onDestroy() = 0;
    
    // 内容视图管理
    void setContentView(View* view);
    View* getContentView() const { return contentView; }
    
    // 状态查询
    ActivityState getState() const { return state; }
    bool isStarted() const;
    bool isResumed() const;
    
    // 生命周期分发方法(由Application调用)
    void dispatchCreate();
    void dispatchStart();
    void dispatchResume();
    void dispatchPause();
    void dispatchStop();
    void dispatchDestroy();

protected:
    View* contentView = nullptr;
    ActivityState state = ActivityState::Destroyed;
}; 