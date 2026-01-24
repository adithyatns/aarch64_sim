add_executable(unit_tests 
  test_registers.cpp 
  test_decoder.cpp 
  test_executor.cpp
  test_memory.cpp
  test_ldr.cpp
  )
target_link_libraries(unit_tests PRIVATE sim_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)
