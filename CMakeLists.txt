cmake_minimum_required(VERSION 3.20)
project(sw)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build the unit tests" ON)

add_subdirectory(swlib)
add_subdirectory(exe)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Optional: Set compiler flags for better diagnostics
if(MSVC)
    target_compile_options(sw PRIVATE /W4)
else()
    target_compile_options(sw PRIVATE -Wall -Wextra -Wpedantic)
endif()
