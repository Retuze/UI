cmake_minimum_required(VERSION 3.15)
project(SimpleGUI VERSION 0.1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FreeType
include(FetchContent)
FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-13-2
)
FetchContent_MakeAvailable(freetype)

# 自动发现所有源文件和头文件
file(GLOB_RECURSE HEADER_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

# 创建库
add_library(simplegui STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

# 设置包含目录
target_include_directories(simplegui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${FREETYPE_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(MINGW)
    # 设置 Windows 版本号
    target_compile_definitions(simplegui 
        PRIVATE 
        WINVER=0x0600 
        _WIN32_WINNT=0x0600
        UNICODE
        _UNICODE
    )
    
    # 链接必要的 Windows 库
    target_link_libraries(simplegui 
        PRIVATE 
        freetype
        gdi32
        user32
        shell32
    )
endif()

# 添加示例目录
add_subdirectory(examples)

# 确保examples目录下的CMakeLists.txt能访问到freetype目标
set(FREETYPE_TARGET freetype CACHE INTERNAL "")

# IDE中的文件分组
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES} ${SOURCE_FILES})

# 生成编译命令数据库
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

# 在examples目录下的target也需要添加FreeType头文件路径
target_include_directories(simple_demo
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${FREETYPE_INCLUDE_DIRS}
) 