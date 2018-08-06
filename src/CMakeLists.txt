
add_executable(TaskLib main.cpp TaskManager.h Task.h)
target_link_libraries(TaskLib pthread)
target_link_libraries(TaskLib Threads::Threads)