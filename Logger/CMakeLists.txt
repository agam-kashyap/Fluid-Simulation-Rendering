file(GLOB LOG_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Log.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Log.cpp"
)

add_library(Log SHARED
"${CMAKE_CURRENT_SOURCE_DIR}/include/Log.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/Log.cpp"
)

target_include_directories(Log PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")