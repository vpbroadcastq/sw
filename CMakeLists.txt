cmake_minimum_required(VERSION 3.20)
project(sw)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# swlib
add_library(swlib STATIC sw.cpp config_path.cpp)

# sw.exe
add_executable(sw main.cpp)
target_link_libraries(sw PRIVATE swlib)

#
#Tests
#
## TODO:  Building tests should be optional
## TODO:  I Think cmake has better testing integration that is better than building a random
#         executable called tests.
find_package(GTest REQUIRED)
add_executable(tests tests/tests.cpp tests/testdata.cpp)
target_link_libraries(tests PRIVATE swlib GTest::GTest GTest::Main)


# Optional: Set compiler flags for better diagnostics
if(MSVC)
    target_compile_options(sw PRIVATE /W4)
else()
    target_compile_options(sw PRIVATE -Wall -Wextra -Wpedantic)
endif()
