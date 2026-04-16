cmake_minimum_required(VERSION 3.20)
project(PointAnalyzer VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fix for CommandLineTools C++ headers issue on macOS
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/Library/Developer/CommandLineTools/SDKs/MacOSX15.4.sdk/usr/include/c++/v1")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCES
    src/main.cpp
    src/point_analyzer.cpp
    src/report_generator.cpp
)

set(HEADERS
    src/point_analyzer.h
    src/report_generator.h
)

add_executable(point_analyzer ${SOURCES} ${HEADERS})

target_include_directories(point_analyzer PRIVATE
    src
    include
)

find_package(Threads REQUIRED)
target_link_libraries(point_analyzer PRIVATE Threads::Threads)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(point_analyzer PRIVATE
        -Wall -Wextra -Wpedantic -O3 -march=native
    )
elseif(MSVC)
    target_compile_options(point_analyzer PRIVATE
        /W4 /O2
    )
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
