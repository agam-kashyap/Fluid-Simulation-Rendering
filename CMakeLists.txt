cmake_minimum_required(VERSION 3.0.0)
project(ACG VERSION 1.0.0)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(.)

add_subdirectory(Logger)
add_subdirectory(SimHelper)
add_subdirectory(Simulation)
add_subdirectory(Render-Marching)
add_subdirectory(SimRun)
add_subdirectory(Ext-Render)
add_subdirectory(Render-ScreenSpace)