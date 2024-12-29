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

# 创建库
add_library(simplegui ${SOURCES})

# 设置包含目录
target_include_directories(simplegui
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
)


# 链接依赖库
target_link_libraries(simplegui
    PUBLIC
        freetype
    PRIVATE
        gdi32
        user32
)

# 安装规则（可选）
install(TARGETS simplegui
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)