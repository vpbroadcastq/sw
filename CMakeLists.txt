cmake_minimum_required(VERSION 3.20)
project(sw)

# Set C++ standard to 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create executable (now builds from two source files)
add_executable(sw sw.cpp config_path.cpp config_file.cpp)

# Optional: Set compiler flags for better diagnostics
if(MSVC)
    target_compile_options(sw PRIVATE /W4)
else()
    target_compile_options(sw PRIVATE -Wall -Wextra -Wpedantic)
endif()
