#pragma once
#include <memory>

class WindowManager;
class RenderContext;
class Application;

class Context {
public:
    virtual ~Context() = default;
    
    // 系统服务获取
    virtual WindowManager* getWindowManager() = 0;
    virtual RenderContext* getRenderContext() = 0;
    virtual Application* getApplication() = 0;
    
    // 资源访问
    virtual std::string getResourcePath() const = 0;
    
    // 权限检查
    virtual bool checkPermission(const std::string& permission) = 0;
}; 