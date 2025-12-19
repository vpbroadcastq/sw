#
#Tests
#
include(GoogleTest)

find_package(GTest REQUIRED)
add_executable(tests tests.cpp testdata.cpp)
target_link_libraries(tests PRIVATE swlib GTest::GTest GTest::Main)

gtest_discover_tests(tests)

