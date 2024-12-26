add_executable(simple_demo
    simple_demo.cpp
)

target_link_libraries(simple_demo
    PRIVATE
        simplegui
        ${FREETYPE_TARGET}
) 