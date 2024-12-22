cmake_minimum_required(VERSION 3.14)
project(UIFramework)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含 FetchContent 模块
include(FetchContent)

# 下载并构建 FreeType
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG        VER-2-13-2  # 使用稳定版本
)

# 配置 FreeType 构建选项
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)

# 构建 FreeType
FetchContent_MakeAvailable(freetype)

# 源文件
set(SOURCES
    src/platform/windows/window.cpp
    src/font/freetype_font.cpp
)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    src/main.cpp
    ${SOURCES}
)

# 添加头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Windows 特定设置
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        UNICODE 
        _UNICODE
    )
    
    target_link_libraries(${PROJECT_NAME} PRIVATE
        gdi32
        freetype
    )
endif() 