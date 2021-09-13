find_package(OpenGL REQUIRED)
find_package(FreeGlutBuild REQUIRED)

include_directories(${GLUT_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

add_executable(ACG main.cpp)

target_link_libraries(ACG renderMarch sim helper ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})

if(UNIX)
    target_link_libraries(${PROJECT_NAME} X11 Xrandr Xi Xxf86vm)
endif()
add_dependencies(ACG freeglut-external)