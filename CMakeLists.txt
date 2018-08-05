cmake_minimum_required(VERSION 3.12)
project(TaskLib)

include_directories(tests/googletest/googletest/include tests/googletest/googletest)

set(CMAKE_CXX_STANDARD 17)

add_executable(TaskLib main.cpp src/TaskManager.h src/Task.h)