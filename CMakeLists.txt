cmake_minimum_required(VERSION 3.16)
project(zuniq)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -pedantic -pipe -march=native)

# Add sanitizer flags for C++ compiler for "Debug" configuration
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer -fsanitize=address,undefined")

# Level 2 optimization for RELEASE mode.
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -g")

enable_testing()
add_executable(zuniq main.cpp zuniq.hpp config.hpp)
add_executable(zuniq_tests tests_runner.cpp zuniq.hpp tests.cpp config.hpp)

add_test(all_tests zuniq_tests)

target_link_libraries(zuniq m)