add_library(sim_core registers.cpp decoder.cpp)
target_include_directories(sim_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
