file(GLOB RENDERMARCH_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Draw.cpp"
)
add_library(
    renderMarch SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Draw.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Draw.cpp"
)

target_link_libraries(renderMarch PUBLIC sim helper)

target_include_directories(renderMarch PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")