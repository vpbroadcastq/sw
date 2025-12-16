#
# swlib
#
add_library(swlib STATIC sw.cpp config_path.cpp)
target_include_directories(swlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

