#pragma once
#include <format>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <windows.h>

// 定义编译条件宏
#ifdef NDEBUG
#define LOG_ENABLED 0
#else
#define LOG_ENABLED 1
#endif

enum class LogLevel { Debug, Info, Warning, Error, Fatal };

class Logger
{
public:
  static Logger& instance()
  {
    static Logger instance;
    return instance;
  }

#if LOG_ENABLED
  void initialize()
  {
    if (!initialized) {
      AllocConsole();
      FILE* dummy;
      freopen_s(&dummy, "CONOUT$", "w", stdout);
      freopen_s(&dummy, "CONOUT$", "w", stderr);
      initialized = true;
      info("Logger", "Logger initialized");
    }
  }

  void setLevel(LogLevel level)
  {
    currentLevel = level;
  }

  void waitForInput()
  {
    if (initialized) {
      system("pause");
    }
  }

  LogLevel getLevel() const
  {
    return currentLevel;
  }

  // 统一的日志接口
  void debug(const char* tag, const char* fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    logImplFormat(LogLevel::Debug, tag, fmt, args);
    va_end(args);
  }

  void info(const char* tag, const char* fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    logImplFormat(LogLevel::Info, tag, fmt, args);
    va_end(args);
  }

  void warning(const char* tag, const char* fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    logImplFormat(LogLevel::Warning, tag, fmt, args);
    va_end(args);
  }

  void error(const char* tag, const char* fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    logImplFormat(LogLevel::Error, tag, fmt, args);
    va_end(args);
  }

  void fatal(const char* tag, const char* fmt, ...)
  {
    va_list args;
    va_start(args, fmt);
    logImplFormat(LogLevel::Fatal, tag, fmt, args);
    va_end(args);
  }

private:
  void logImplFormat(LogLevel level, const char* tag, const char* fmt, va_list args)
  {
    if (level < currentLevel || !initialized)
      return;

    // 首先计算需要的缓冲区大小
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    if (size < 0) return;  // 格式化错误

    // 分配足够的空间并执行实际的格式化
    std::string buffer(size + 1, '\0');
    vsnprintf(buffer.data(), buffer.size(), fmt, args);

    logMessage(level, tag, buffer);
  }

  bool initialized = false;
  LogLevel currentLevel = LogLevel::Info;
  std::mutex mutex;
#else
  // 移动成员变量到这里，这样在禁用日志时也能编译
  bool initialized = false;
  LogLevel currentLevel = LogLevel::Info;

  void initialize() {}
  void setLevel(LogLevel level)
  {
    currentLevel = level;
  }
  LogLevel getLevel() const
  {
    return currentLevel;
  }
  void waitForInput() {}

  void debug(const char*, const char*, ...) {}
  void info(const char*, const char*, ...) {}
  void warning(const char*, const char*, ...) {}
  void error(const char*, const char*, ...) {}
  void fatal(const char*, const char*, ...) {}
#endif

private:
  Logger() = default;

  const char* getLevelString(LogLevel level) const
  {
    switch (level) {
    case LogLevel::Debug:
      return "DEBUG";
    case LogLevel::Info:
      return "INFO";
    case LogLevel::Warning:
      return "WARN";
    case LogLevel::Error:
      return "ERROR";
    case LogLevel::Fatal:
      return "FATAL";
    default:
      return "UNKNOWN";
    }
  }

  void logMessage(LogLevel level, const char* tag, const std::string& message)
  {
    if (level < currentLevel || !initialized)
        return;

    std::lock_guard<std::mutex> lock(mutex);
    
    // 直接构建格式化的消息
    const char* levelStr = getLevelString(level);
    char prefix[64];
    int padding = 5 - static_cast<int>(strlen(levelStr));
    snprintf(prefix, sizeof(prefix), "[%s]%*s[%s] ", 
             levelStr, padding, "", tag);

    // 转换为宽字符
    std::string output = prefix + message + "\n";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, output.c_str(),
                                        (int)output.length(), nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, output.c_str(), (int)output.length(), 
                        &wstr[0], size_needed);

    // 输出到控制台
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleW(hConsole, wstr.c_str(), wstr.length(), nullptr, nullptr);
  }
};

// 定义日志宏
#if LOG_ENABLED
#define LOG_TAG(tag) static constexpr const char* LOG_TAG = tag
#define LOGD(...) Logger::instance().debug(LOG_TAG, __VA_ARGS__)
#define LOGI(...) Logger::instance().info(LOG_TAG, __VA_ARGS__)
#define LOGW(...) Logger::instance().warning(LOG_TAG, __VA_ARGS__)
#define LOGE(...) Logger::instance().error(LOG_TAG, __VA_ARGS__)
#define LOGF(...) Logger::instance().fatal(LOG_TAG, __VA_ARGS__)
#else
#define LOG_TAG(tag)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#define LOGF(...)
#endif


