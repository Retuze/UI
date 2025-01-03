#pragma once
#include "view/view.h"
#include "view/linear_layout.h"
#include "view/view_root_impl.h"
#include <memory>

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

    Activity();
    virtual ~Activity() = default;
    
    // 生命周期方法
    virtual void onCreate() {}
    virtual void onStart() {}
    virtual void onResume() {}
    virtual void onPause() {}
    virtual void onStop() {}
    virtual void onDestroy() {}
    
    // 内容视图管理
    void setContentView(View* view);
    void setContentView(View* view, const LayoutParams& params);
    View* getContentView() const { return rootLayout.get(); }
    LinearLayout* getRootLayout() const { return rootLayout.get(); }
    
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
    
    void performTraversal() { viewRoot.performTraversal(); }
    
protected:
    ActivityState state = ActivityState::Created;
    View* contentView = nullptr;
    std::unique_ptr<LinearLayout> rootLayout;
    ViewRootImpl viewRoot;
}; 