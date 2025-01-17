cmake_minimum_required(VERSION 3.15)
project(SimpleGUI VERSION 0.1.0 LANGUAGES CXX)

# 设置 CMake 策略
cmake_policy(SET CMP0077 NEW)
# 设置 Python 相关策略
cmake_policy(SET CMP0148 NEW)

# 启用 compile_commands.json 生成
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含 FetchContent 模块
include(FetchContent)

# FreeType 配置
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)

# 禁用警告
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
set(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION OFF CACHE BOOL "" FORCE)

# 配置并下载 FreeType
if(NOT TARGET freetype)
    FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG        VER-2-13-2
        GIT_SHALLOW    ON
        UPDATE_DISCONNECTED ON
    )
    FetchContent_MakeAvailable(freetype)
endif()

# HarfBuzz 配置
set(HB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(HB_BUILD_UTILS OFF CACHE BOOL "" FORCE)
set(HB_BUILD_SUBSET OFF CACHE BOOL "" FORCE)
set(HB_HAVE_FREETYPE ON CACHE BOOL "" FORCE)
set(HB_HAVE_INTROSPECTION OFF CACHE BOOL "" FORCE)

# 配置并下载 HarfBuzz
if(NOT TARGET harfbuzz)
    FetchContent_Declare(
        harfbuzz
        GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
        GIT_TAG 8.2.1
        GIT_SHALLOW ON
        UPDATE_DISCONNECTED ON
    )
    FetchContent_MakeAvailable(harfbuzz)
endif()

# Windows 相关定义
add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00)

# 添加子目录（让src/CMakeLists.txt处理simplegui的构建）
add_subdirectory(src)

# 构建示例
add_executable(demo examples/demo.cpp)
target_link_libraries(demo PRIVATE simplegui)

# 生成map文件
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_BINARY_DIR}/demo.map")