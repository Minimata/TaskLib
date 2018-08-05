
#include <iostream>
#include "src/TaskManager.h"

int main(int argc, char** argv) {
    
    TaskLib::TaskManager taskManager;
    
    std::cout << "Task manager created" << std::endl;
    auto myFirstTaskID = taskManager.createTask([](){std::cout << "hello from lambda" << std::endl;});
    std::cout << "Task created : " << myFirstTaskID << std::endl;
    
    taskManager.startAllTasks();
    taskManager.pauseAllTasks();
    taskManager.resumeAllTasks();
    taskManager.stopAllTasks();
    
    return 0;
}