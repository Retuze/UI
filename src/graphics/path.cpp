#include "graphics/path.h"

void Path::moveTo(float x, float y) {
    commands.push_back({CommandType::MoveTo, Point(x, y)});
}

void Path::lineTo(float x, float y) {
    commands.push_back({CommandType::LineTo, Point(x, y)});
}

void Path::close() {
    commands.push_back({CommandType::Close, Point()});
} 