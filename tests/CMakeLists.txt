include(GoogleTest)

add_subdirectory(googletest)
include_directories(googletest/googletest/include googletest/googletest)

add_executable(RunAllTests RunAllTests.cpp TaskManagerTests.h TaskTests.h)
target_link_libraries(RunAllTests gtest gtest_main pthread Threads::Threads)

gtest_discover_tests(RunAllTests)