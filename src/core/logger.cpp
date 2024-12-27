 #include "core/logger.h"

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::initialize() {
    if (!initialized) {
        AllocConsole();
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
        initialized = true;
        info("Logger", "Logger initialized");
    }
}

void Logger::setLevel(LogLevel level) {
    currentLevel = level;
}

void Logger::waitForInput() {
    if (initialized) {
        system("pause");
    }
}

const char* Logger::getLevelString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Fatal:   return "FATAL";
        default:               return "UNKNOWN";
    }
}