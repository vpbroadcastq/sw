#
#Tests
#

## TODO:  Building tests should be optional
## TODO:  I Think cmake has better testing integration that is better than building a random
#         executable called tests.
find_package(GTest REQUIRED)
add_executable(tests tests.cpp testdata.cpp)
target_link_libraries(tests PRIVATE swlib GTest::GTest GTest::Main)

