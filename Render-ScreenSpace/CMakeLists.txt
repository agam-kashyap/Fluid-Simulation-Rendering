file(GLOB RENDER_FILES
"${CMAKE_CURRENT_SOURCE_DIR}/include/shader.h"
"${CMAKE_CURRENT_SOURCE_DIR}/include/camera.h"
"${CMAKE_CURRENT_SOURCE_DIR}/include/glad.c"
"${CMAKE_CURRENT_SOURCE_DIR}/include/sphere.h"
"${CMAKE_CURRENT_SOURCE_DIR}/src/shader.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/sphere.cpp"
)

add_executable(ACG main.cpp ${RENDER_FILES} ${SIMULATION_FILES} ${SIMHELPER_FILES})
target_link_libraries(ACG sim helper)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} glfw3 GL X11 pthread Xrandr Xi dl)
    set_target_properties(ACG PROPERTIES COMPILE_FLAGS "-fopenmp" )
endif()