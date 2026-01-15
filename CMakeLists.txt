cmake_minimum_required(VERSION 3.14)
project(AArch64_Simulator VERSION 0.1.0 LANGUAGES CXX)

# Standard C++ for Systems Programming
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Fetch GoogleTest automatically
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For Windows compatibility (though you are on Linux, it's good practice)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 2. Add your subdirectories
enable_testing()
add_subdirectory(src)
add_subdirectory(tests)

# 3. Static Analysis Integration (Clang-Tidy)
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=bugprone-*,modernize-*,readability-*,performance-*")
endif()
