cmake_minimum_required(VERSION 3.0.0)
project(ACG VERSION 0.1.0)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(.)

add_subdirectory(SimHelper)
add_subdirectory(Simulation)
add_subdirectory(Render-Marching)
add_subdirectory(Project)
add_subdirectory(SimRun)