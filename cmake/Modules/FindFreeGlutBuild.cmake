include(ExternalProject)

if (UNIX)
    ExternalProject_Add(freeglut-external
      PREFIX ${CMAKE_BINARY_DIR}
      URL ${PROJECT_SOURCE_DIR}/freeglut
      CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install -DFREEGLUT_BUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      TEST_COMMAND ""
    )

    set(GLUT_LIBRARIES ${CMAKE_BINARY_DIR}/install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}glut${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(GLUT_INCLUDE_DIR ${CMAKE_BINARY_DIR}/install/include)
endif()
