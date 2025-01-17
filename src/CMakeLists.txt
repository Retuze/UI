# 自动查找所有源文件，并在文件变化时自动重新配置
file(GLOB_RECURSE SOURCES 
    CONFIGURE_DEPENDS
    "*.cpp"
    "activity/*.cpp"
    "core/*.cpp"
    "graphics/*.cpp"
    "layout/*.cpp"
    "widgets/*.cpp"
)

# 创建静态库
add_library(simplegui STATIC ${SOURCES})

# 设置包含目录
target_include_directories(simplegui
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/_deps/freetype-src/include
        ${CMAKE_SOURCE_DIR}/_deps/harfbuzz-src/include
)

if(WIN32)
    # 检查编译器类型
    if(MSVC)
        target_link_libraries(simplegui PRIVATE dwmapi)
    else()
        # MinGW 和 LLVM 需要使用 -ldwmapi
        target_link_libraries(simplegui PRIVATE -ldwmapi)
    endif()
endif()

# 链接依赖库
target_link_libraries(simplegui
    PUBLIC
        freetype
        harfbuzz
    PRIVATE
        gdi32
        user32
        -static
)

# 安装规则
install(TARGETS simplegui
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)