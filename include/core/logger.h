#pragma once
#include <windows.h>
#include <iostream>
#include <sstream>
#include <mutex>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class Logger {
public:
    static Logger& getInstance();
    
    void initialize();
    void setLevel(LogLevel level);
    LogLevel getLevel() const { return currentLevel; }
    
    template<typename... Args>
    void debug(const char* tag, Args&&... args) {
        log(LogLevel::Debug, tag, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void info(const char* tag, Args&&... args) {
        log(LogLevel::Info, tag, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void warning(const char* tag, Args&&... args) {
        log(LogLevel::Warning, tag, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void error(const char* tag, Args&&... args) {
        log(LogLevel::Error, tag, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void fatal(const char* tag, Args&&... args) {
        log(LogLevel::Fatal, tag, std::forward<Args>(args)...);
    }
    
    void waitForInput();

private:
    Logger() = default;
    
    template<typename... Args>
    void log(LogLevel level, const char* tag, Args&&... args) {
        if (level < currentLevel) return;
        if (!initialized) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "[" << getLevelString(level) << "] "
           << "[" << tag << "] ";
        (ss << ... << std::forward<Args>(args));
        std::cout << ss.str() << std::endl;
    }
    
    const char* getLevelString(LogLevel level) const;
    
    bool initialized = false;
    LogLevel currentLevel = LogLevel::Info;
    std::mutex mutex;
};

#define LOG_TAG(tag) static constexpr const char* LOG_TAG = tag

// 便捷宏
#define LOGD(...) Logger::getInstance().debug(LOG_TAG, __VA_ARGS__)
#define LOGI(...) Logger::getInstance().info(LOG_TAG, __VA_ARGS__)
#define LOGW(...) Logger::getInstance().warning(LOG_TAG, __VA_ARGS__)
#define LOGE(...) Logger::getInstance().error(LOG_TAG, __VA_ARGS__)
#define LOGF(...) Logger::getInstance().fatal(LOG_TAG, __VA_ARGS__) 