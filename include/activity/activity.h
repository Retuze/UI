#pragma once
#include "core/context.h"
#include "view/view.h"
#include <memory>

class Activity : public Context {
public:
    enum class ActivityState {
        Created, Started, Resumed, Paused, Stopped, Destroyed
    };
    
    Activity();
    virtual ~Activity() = default;
    
    // Context接口实现
    WindowManager* getWindowManager() override;
    RenderContext* getRenderContext() override;
    Application* getApplication() override;
    std::string getResourcePath() const override;
    bool checkPermission(const std::string& permission) override;
    
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
    View* getContentView() const { return contentView; }
    
    bool isStarted() const;
    bool isResumed() const;
    
protected:
    friend class Application;
    void dispatchCreate();
    void dispatchStart();
    void dispatchResume();
    void dispatchPause();
    void dispatchStop();
    void dispatchDestroy();
    
private:
    ActivityState state = ActivityState::Created;
    View* contentView = nullptr;
}; 