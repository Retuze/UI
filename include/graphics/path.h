#pragma once
#include "core/types.h"
#include <vector>

class Path {
public:
    struct Point {
        float x, y;
        Point(float x = 0, float y = 0) : x(x), y(y) {}
    };
    
    enum CommandType {
        MoveTo,
        LineTo,
        Close
    };
    
    struct Command {
        CommandType type;
        Point point;
    };
    
    void moveTo(float x, float y);
    void lineTo(float x, float y);
    void close();
    
    const std::vector<Command>& getCommands() const { return commands; }
    
private:
    std::vector<Command> commands;
}; 