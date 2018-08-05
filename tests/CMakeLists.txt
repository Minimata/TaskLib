
# enable_testing()
add_subdirectory(googletest)

include_directories(googletest/googletest/include googletest/googletest)

add_executable(RunAllTests RunAllTests.cpp TaskManagerTests.h)

target_link_libraries(RunAllTests gtest gtest_main)
# add_test(AllTests RunAllTests)