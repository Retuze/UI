cmake_minimum_required(VERSION 3.10)
project(UIFramework)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 源文件
set(SOURCES
    src/platform/windows/window.cpp
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
    )
endif() 