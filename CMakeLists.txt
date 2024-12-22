cmake_minimum_required(VERSION 3.15)
project(image_viewer)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # 生成compile_commands.json

# 添加源文件
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/image_viewer.cpp
)

# 添加头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Windows特定设置
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        gdi32
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        UNICODE
        _UNICODE
    )
endif() 