file(GLOB SIMULATION_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/collisions.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/define.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/forces.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/iisph-forces.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/particle.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/sph.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/collisions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/forces.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/iisph-forces.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/particle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sph.cpp"
)
add_library(
    sim SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/include/sph.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sph.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/config.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/forces.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/forces.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/iisph-forces.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/iisph-forces.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/particle.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/particle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/define.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/collisions.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/collisions.cpp"
    )
target_link_libraries(sim PUBLIC helper)

target_include_directories(sim PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")