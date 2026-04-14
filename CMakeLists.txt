cmake_minimum_required(VERSION 3.20)
project(PointAnalyzer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source files
set(SOURCES
    src/main.cpp
    src/point_analyzer.cpp
    src/report_generator.cpp
)

set(HEADERS
    src/point_analyzer.h
    src/report_generator.h
)

# Executable
add_executable(point_analyzer ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(point_analyzer PRIVATE
    src
    include
)

# Compiler optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(point_analyzer PRIVATE
        -Wall -Wextra -O3 -march=native
    )
elseif(MSVC)
    target_compile_options(point_analyzer PRIVATE
        /W3 /O2
    )
endif()

# Testing
enable_testing()
find_package(GTest REQUIRED)

set(TEST_SOURCES
    tests/test_point_analyzer.cpp
)

add_executable(test_point_analyzer ${TEST_SOURCES})
target_link_libraries(test_point_analyzer GTest::gtest_main)
target_include_directories(test_point_analyzer PRIVATE src include)

include(GoogleTest)
gtest_discover_tests(test_point_analyzer)
