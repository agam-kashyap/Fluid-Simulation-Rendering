add_executable(sim-run main.cpp)           

find_package(glog 0.6.0 REQUIRED)

target_link_libraries(sim-run PUBLIC sim helper glog::glog)
set_target_properties(sim-run PROPERTIES COMPILE_FLAGS "-fopenmp" )