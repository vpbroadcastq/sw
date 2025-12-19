#
# swlib
#
set(SWLIB_SOURCES
    sw.cpp
)

if (WIN32)
    list(APPEND SWLIB_SOURCES config_path_win.cpp)
elseif (UNIX)
    list(APPEND SWLIB_SOURCES config_path_unix.cpp)
endif()

add_library(swlib STATIC ${SWLIB_SOURCES})

target_include_directories(swlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

