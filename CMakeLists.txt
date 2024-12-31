cmake_minimum_required(VERSION 3.15)
project(SimpleGUI VERSION 0.1.0 LANGUAGES CXX)

# 设置 CMake 策略来解决警告
cmake_policy(SET CMP0077 NEW)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含 FetchContent 模块
include(FetchContent)

# 在下载 FreeType 之前设置所有选项
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)

# 禁用警告
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
set(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION OFF CACHE BOOL "" FORCE)

# 配置并下载 FreeType
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG        VER-2-13-2
)

# 下载并构建 FreeType
FetchContent_MakeAvailable(freetype)

# 添加 Windows Unicode 支持
add_definitions(-DUNICODE -D_UNICODE)

# 设置最低 Windows 版本为 Vista
add_definitions(-D_WIN32_WINNT=0x0600 -DWINVER=0x0600)

# 添加子目录
add_subdirectory(src)

# 构建示例  
add_executable(surface_test examples/surface_test.cpp)
target_link_libraries(surface_test PRIVATE simplegui)