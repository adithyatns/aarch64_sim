add_executable(unit_tests test_registers.cpp)
target_link_libraries(unit_tests PRIVATE sim_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)
