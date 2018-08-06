cmake_minimum_required(VERSION 3.12)
project(TaskLib)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_subdirectory(src)
add_subdirectory(tests)

