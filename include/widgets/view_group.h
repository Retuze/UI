#pragma once
#include "widgets/widget.h"
#include "layout/layout.h"

class ViewGroup : public Widget {
public:
    ViewGroup();
    ~ViewGroup() override;
    
    // 子控件管理
    void addView(Widget* child);
    void removeView(Widget* child);
    void removeAllViews();
    
    // 布局相关
    void setLayout(Layout* layout);
    void requestLayout();
    
    // 绘制
    void draw(Renderer* renderer) override;
    
    // 事件处理
    bool dispatchEvent(const Event& event) override;
    virtual bool onInterceptEvent(const Event& event) override { return false; }
    
    void post(Runnable&& runnable) override {
        postDelayed(std::move(runnable), std::chrono::milliseconds(0));
    }
    
    void postDelayed(Runnable&& runnable, std::chrono::milliseconds delay) override {
        auto when = std::chrono::steady_clock::now() + delay;
        std::lock_guard<std::mutex> lock(queueMutex);
        messageQueue.push_back({std::move(runnable), when});
    }
    
    // 添加重复执行的消息
    void postRepeating(Runnable&& runnable, std::chrono::milliseconds interval, const void* token = nullptr) {
        auto when = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> lock(queueMutex);
        messageQueue.push_back({std::move(runnable), when, true, interval, token});
    }
    
    void removeCallbacks(const void* token) {
        std::lock_guard<std::mutex> lock(queueMutex);
        messageQueue.erase(
            std::remove_if(messageQueue.begin(), messageQueue.end(),
                [token](const Message& msg) {
                    return msg.token == token;
                }),
            messageQueue.end());
    }
    
    // 在渲染前处理消息队列
    void processMessages() {
        auto now = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> lock(queueMutex);
        
        auto it = messageQueue.begin();
        while (it != messageQueue.end()) {
            if (it->when <= now) {
                it->runnable();
                it = messageQueue.erase(it);
            } else {
                ++it;
            }
        }
    }

protected:
    Layout* layout;
    std::vector<Widget*> children;
    bool needsLayout;
    
    // 执行布局
    virtual void onLayout();
    
    struct Message {
        Runnable runnable;
        std::chrono::steady_clock::time_point when;
        bool repeat = false;
        std::chrono::milliseconds interval{0};
        const void* token = nullptr;
    };
    
    std::vector<Message> messageQueue;
    std::mutex queueMutex;
}; 