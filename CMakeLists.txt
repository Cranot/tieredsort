cmake_minimum_required(VERSION 3.14)
project(tieredsort VERSION 1.0.1 LANGUAGES CXX)

# Header-only library
add_library(tieredsort INTERFACE)
target_include_directories(tieredsort INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(tieredsort INTERFACE cxx_std_17)

# Options
option(TIEREDSORT_BUILD_TESTS "Build tests" OFF)
option(TIEREDSORT_BUILD_BENCHMARKS "Build benchmarks" OFF)

# Tests
if(TIEREDSORT_BUILD_TESTS)
    enable_testing()
    add_executable(test_tieredsort tests/test_tieredsort.cpp)
    target_link_libraries(test_tieredsort PRIVATE tieredsort)
    add_test(NAME tieredsort_tests COMMAND test_tieredsort)
endif()

# Benchmarks
if(TIEREDSORT_BUILD_BENCHMARKS)
    add_executable(benchmark benchmark/benchmark.cpp)
    target_link_libraries(benchmark PRIVATE tieredsort)
endif()

# Install
install(TARGETS tieredsort EXPORT tieredsortTargets)
install(FILES include/tieredsort.hpp DESTINATION include)
