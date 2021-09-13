file(GLOB SIMHELPER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Volume.cpp"
)
add_library(helper SHARED 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/DataStructures.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/point.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/point.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Obstacles.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Octree.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Volume.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Volume.cpp"
)

target_include_directories(helper PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")