cmake_minimum_required(VERSION 3.0.0)
project(ACG VERSION 0.1.0)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")


# set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lXi")
# set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lXxf86vm")
# set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -lX11")
# set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -Xrandr")

include_directories(.)

add_subdirectory(freeglut)
add_subdirectory(SimHelper)
set_target_properties(helper PROPERTIES LINKER_LANGUAGE CXX)
add_subdirectory(Simulation)
add_subdirectory(Render-Marching)
add_subdirectory(Project)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
