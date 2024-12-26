cmake_minimum_required(VERSION 3.15)
project(SimpleGUI VERSION 0.1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FreeType
include(FetchContent)
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-13-2
)
FetchContent_MakeAvailable(freetype)

# 自动发现头文件
file(GLOB_RECURSE HEADER_FILES
    "include/*.h"
    "include/*.hpp"
)

# 自动发现源文件
file(GLOB_RECURSE SOURCE_FILES
    "src/*.cpp"
    "src/*.cc"
)

# 创建库
add_library(simplegui STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

# 设置包含目录和链接
target_include_directories(simplegui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(simplegui PRIVATE freetype)

# 添加示例目录
add_subdirectory(examples)

# IDE中的文件分组
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES} ${SOURCE_FILES}) 

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 