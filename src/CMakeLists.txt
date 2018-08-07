
add_executable(TaskLib main.cpp TaskHandling/TaskManager.h TaskHandling/Task.h)
target_link_libraries(TaskLib pthread)
target_link_libraries(TaskLib Threads::Threads)