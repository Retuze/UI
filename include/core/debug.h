#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>

class Debug {
public:
    static void Initialize() {
        if (!isInitialized) {
            AllocConsole();
            FILE* dummy;
            freopen_s(&dummy, "CONOUT$", "w", stdout);
            freopen_s(&dummy, "CONOUT$", "w", stderr);
            isInitialized = true;
            Log("Debug console initialized");
        }
    }
    
    template<typename... Args>
    static void Log(const Args&... args) {
        std::stringstream ss;
        (ss << ... << args);
        std::cout << "[LOG] " << ss.str() << std::endl;
    }
    
    template<typename... Args>
    static void Error(const Args&... args) {
        std::stringstream ss;
        (ss << ... << args);
        std::cerr << "[ERROR] " << ss.str() << std::endl;
    }
    
    static void WaitForInput() {
        if (isInitialized) {
            system("pause");
        }
    }

private:
    static bool isInitialized;
}; 