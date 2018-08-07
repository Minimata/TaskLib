
#include <iostream>
#include "TaskHandling/TaskManager.h"

#include <unordered_map>
#include <string>
#include <functional>

int main(int argc, char** argv) {
    
    TaskLib::TaskManager taskManager;
    
    auto myFirstTaskID = taskManager.createTask([](){
        std::cout << std::endl;
        std::cout << "Subroutine started." << std::endl;
        int i = INT32_MAX;
        while(i > 0) {
            i--;
        }
    }, [](){ std::cout << "Subroutine ended." << std::endl; });
    
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