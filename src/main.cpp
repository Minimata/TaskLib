
#include <iostream>
#include "TaskManager.h"

#include <unordered_map>
#include <string>
#include <functional>

int main(int argc, char** argv) {
    
    TaskLib::TaskManager taskManager;
    
    std::cout << "Task manager created" << std::endl;
    auto myFirstTaskID = taskManager.createTask([](){
        std::cout << "Subroutine started." << std::endl;
        int i = 20;
        while(i > 0) i--;
        std::cout << "Subroutine ended." << std::endl;
    });
    std::cout << "Task created : " << myFirstTaskID << std::endl;
    
    bool canContinue = true;
    std::unordered_map<std::string, std::function<void ()>> actions = {
            {"quit", [&](){canContinue = false; std::cout << "Quitting..." << std::endl; taskManager.stopAllTasks();}},
            {"start", [&](){taskManager.startAllTasks();}},
            {"pause", [&](){taskManager.pauseAllTasks();}},
            {"resume", [&](){taskManager.resumeAllTasks();}},
            {"stop", [&](){taskManager.stopAllTasks();}},
    };
    
    std::string command;
    while(canContinue) {
        std::cout << "Enter a command : ";
        std::cin >> command;
        std::cout << std::endl;
        
        if(actions.find(command) != actions.end()) actions.at(command)();
        else std::cout << "Unavailable command" << std::endl;
    }
    
    return 0;
}