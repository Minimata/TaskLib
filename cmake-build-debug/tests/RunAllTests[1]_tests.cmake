add_test( TaskManagerFixture.ValidCreateTask D:/Projects/CPP/TaskLib/cmake-build-debug/tests/RunAllTests.exe [==[--gtest_filter=TaskManagerFixture.ValidCreateTask]==] --gtest_also_run_disabled_tests)
set_tests_properties( TaskManagerFixture.ValidCreateTask PROPERTIES WORKING_DIRECTORY D:/Projects/CPP/TaskLib/cmake-build-debug/tests)
add_test( TaskManagerFixture.ValidTaskStates D:/Projects/CPP/TaskLib/cmake-build-debug/tests/RunAllTests.exe [==[--gtest_filter=TaskManagerFixture.ValidTaskStates]==] --gtest_also_run_disabled_tests)
set_tests_properties( TaskManagerFixture.ValidTaskStates PROPERTIES WORKING_DIRECTORY D:/Projects/CPP/TaskLib/cmake-build-debug/tests)
add_test( TaskFixture.ValidStateChanges D:/Projects/CPP/TaskLib/cmake-build-debug/tests/RunAllTests.exe [==[--gtest_filter=TaskFixture.ValidStateChanges]==] --gtest_also_run_disabled_tests)
set_tests_properties( TaskFixture.ValidStateChanges PROPERTIES WORKING_DIRECTORY D:/Projects/CPP/TaskLib/cmake-build-debug/tests)
set( RunAllTests_TESTS TaskManagerFixture.ValidCreateTask TaskManagerFixture.ValidTaskStates TaskFixture.ValidStateChanges)
