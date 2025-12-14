cmake_minimum_required(VERSION 3.20)
project(sw)

# Set C++ standard to 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Create executable (now builds from two source files)
add_executable(sw sw.cpp config_path.cpp config_file.cpp)

#
#Tests
#
## TODO:  Building tests should be optional
## TODO:  I should build the fn's being tested into a standalone lib and consume them from
#         both the main executable and the tests.
##TODO:   I Think cmake has better testing integration that is better than building a random
#         executable called tests.
find_package(GTest REQUIRED)
add_executable(tests tests/tests.cpp config_file.cpp)
target_link_libraries(tests PRIVATE GTest::GTest GTest::Main)

# Copy the test data to the build directory
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")



# Optional: Set compiler flags for better diagnostics
if(MSVC)
    target_compile_options(sw PRIVATE /W4)
else()
    target_compile_options(sw PRIVATE -Wall -Wextra -Wpedantic)
endif()
