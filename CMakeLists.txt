cmake_minimum_required(VERSION 3.0.0)
project(ACG VERSION 0.1.0)

include_directories(.)

add_subdirectory(SimHelper)
add_subdirectory(Simulation)
add_subdirectory(Render-Marching)
add_subdirectory(Project)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
