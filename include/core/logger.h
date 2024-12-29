#pragma once
#include <windows.h>
#include <iostream>
#include <sstream>
#include <mutex>

// 定义编译条件宏
#ifdef NDEBUG
#define LOG_ENABLED 0
#else
#define LOG_ENABLED 1
#endif

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
    
#if LOG_ENABLED
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
#else
    void initialize() {}
    void setLevel(LogLevel) {}
    LogLevel getLevel() const { return LogLevel::Info; }
    void waitForInput() {}
    
    template<typename... Args>
    void debug(const char*, Args&&...) {}
    template<typename... Args>
    void info(const char*, Args&&...) {}
    template<typename... Args>
    void warning(const char*, Args&&...) {}
    template<typename... Args>
    void error(const char*, Args&&...) {}
    template<typename... Args>
    void fatal(const char*, Args&&...) {}
#endif

private:
    Logger() = default;
    
#if LOG_ENABLED
    template<typename... Args>
    void log(LogLevel level, const char* tag, Args&&... args) {
        if (level < currentLevel) return;
        if (!initialized) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        std::stringstream ss;
        ss << "[" << getLevelString(level) << "] "
           << "[" << tag << "] ";
        (ss << ... << std::forward<Args>(args));
        
        std::string str = ss.str();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), nullptr, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstr[0], size_needed);
        
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        WriteConsoleW(hConsole, wstr.c_str(), wstr.length(), nullptr, nullptr);
        WriteConsoleW(hConsole, L"\n", 1, nullptr, nullptr);
    }
    
    const char* getLevelString(LogLevel level) const;
    
    bool initialized = false;
    LogLevel currentLevel = LogLevel::Info;
    std::mutex mutex;
#endif
};

// 根据编译条件定义日志宏
#if LOG_ENABLED
    #define LOG_TAG(tag) static constexpr const char* LOG_TAG = tag
    #define LOGD(...) Logger::getInstance().debug(LOG_TAG, __VA_ARGS__)
    #define LOGI(...) Logger::getInstance().info(LOG_TAG, __VA_ARGS__)
    #define LOGW(...) Logger::getInstance().warning(LOG_TAG, __VA_ARGS__)
    #define LOGE(...) Logger::getInstance().error(LOG_TAG, __VA_ARGS__)
    #define LOGF(...) Logger::getInstance().fatal(LOG_TAG, __VA_ARGS__)
#else
    #define LOG_TAG(tag)
    #define LOGD(...)
    #define LOGI(...)
    #define LOGW(...)
    #define LOGE(...)
    #define LOGF(...)
#endif 