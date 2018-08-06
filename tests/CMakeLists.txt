include(GoogleTest)

add_subdirectory(googletest)
include_directories(googletest/googletest/include googletest/googletest)

add_executable(RunAllTests RunAllTests.cpp TaskManagerTests.h)
target_link_libraries(RunAllTests gtest gtest_main)

# gtest_add_tests(TARGET RunAllTests)
gtest_discover_tests(RunAllTests)