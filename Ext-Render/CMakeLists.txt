add_executable(EREND main.cpp ${RENDERMARCH_FILES} ${SIMULATION_FILES} ${SIMHELPER_FILES} )

target_link_libraries(EREND renderMarch sim helper)

if(UNIX)
    target_link_libraries(EREND X11 Xrandr Xi Xxf86vm glut GL GLU)
    set_target_properties(EREND PROPERTIES COMPILE_FLAGS "-fopenmp" )
endif()