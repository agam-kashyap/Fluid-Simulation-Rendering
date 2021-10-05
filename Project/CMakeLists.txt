add_executable(ACG main.cpp ${RENDERMARCH_FILES} ${SIMULATION_FILES} ${SIMHELPER_FILES} )

target_link_libraries(ACG renderMarch sim helper)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} X11 Xrandr Xi Xxf86vm glut GL GLU)
    set_target_properties(ACG PROPERTIES COMPILE_FLAGS "-fopenmp" )
endif()