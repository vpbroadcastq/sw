cmake_minimum_required(VERSION 3.20)
project(sw VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build the unit tests" ON)

add_subdirectory(swlib)
add_subdirectory(exe)

if(BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

#
# Install logic
#
#include(GNUInstallDirs)
# Install only the executable
#install(TARGETS myproj
#  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # typically bin/
#)
# Optional: allow "cmake --install" to strip in Release installs
# set(CMAKE_INSTALL_DO_STRIP TRUE)


# Optional: Set compiler flags for better diagnostics
if(MSVC)
    target_compile_options(sw PRIVATE /W4)
else()
    target_compile_options(sw PRIVATE -Wall -Wextra -Wpedantic)
endif()

