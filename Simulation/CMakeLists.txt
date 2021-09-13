file(GLOB SIMULATION_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sph.cpp"
)
add_library(
    sim SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/sph.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sph.cpp"
    )
target_link_libraries(sim PUBLIC helper)

target_include_directories(sim PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")