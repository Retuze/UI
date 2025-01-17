#pragma once
#include "core/context.h"
#include "view/view.h"
#include <memory>
#include <any>
#include <map>

class Activity : public Context {
public:
    enum class ActivityState {
        Created,    // Activity已创建但未启动
        Started,    // Activity可见但未获得焦点
        Resumed,    // Activity可见且具有焦点
        Paused,     // Activity失去焦点但仍可见
        Stopped,    // Activity完全不可见（可能在后台）
        Destroyed   // Activity已销毁
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
    
    // 后台相关的生命周期方法
    virtual void onSaveInstanceState() {}    // 保存Activity状态
    virtual void onRestoreInstanceState() {} // 恢复Activity状态
    virtual void onTrimMemory(int level) {} // 响应内存清理请求
    
    // 内容视图管理
    void setContentView(View* view);
    void setContentView(View* view, const LayoutParams& params);
    View* getContentView() const { return contentView; }
    
    // 状态查询
    bool isStarted() const { return state >= ActivityState::Started; }
    bool isResumed() const { return state == ActivityState::Resumed; }
    bool isVisible() const { return state >= ActivityState::Started && state <= ActivityState::Paused; }
    ActivityState getState() const { return state; }
    
    void finish();
    
    // 获取启动此Activity时传递的数据
    template<typename T>
    T getExtra(const std::string& key, const T& defaultValue = T()) const {
        if (extras.count(key) > 0) {
            try {
                return std::any_cast<T>(extras.at(key));
            } catch (const std::bad_any_cast&) {
                return defaultValue;
            }
        }
        return defaultValue;
    }

protected:
    friend class Application;
    void dispatchCreate();
    void dispatchStart();
    void dispatchResume();
    void dispatchPause();
    void dispatchStop();
    void dispatchDestroy();
    void dispatchSaveInstanceState();
    void dispatchRestoreInstanceState();
    
    // Activity结果回调
    virtual void onActivityResult(int requestCode, int resultCode, const std::map<std::string, std::any>& data) {}
    
private:
    ActivityState state = ActivityState::Created;
    View* contentView = nullptr;
    bool isStateSaved = false;
    std::map<std::string, std::any> extras;        // 启动时携带的数据
}; 