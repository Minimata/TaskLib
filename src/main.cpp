
#include <iostream>
#include "TaskHandling/TaskManager.h"

#include <unordered_map>
#include <string>
#include <functional>
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>
#include <iterator>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "../thread/mingw.thread.h"
#endif

void displayHelp() {
    std::cout   << "Welcome to the TaskLib example program !\n\n\n"
    
    
                << "\t - help : displays this message.\n"
                << "\t - quit : detach threads still running and shuts down program.\n\n"
                
                << "\t - start : Start to launch a rocket!\n"
                << std::endl;
}

int main(int argc, char** argv) {
    
    if(argc > 1) {
        if(argv[1] == std::string("--help")) {
            displayHelp();
            return 0;
        }
        else {
            std::cerr << "Invalid arguments. launch with '--help' for instructions." << std::endl;
            return 1;
        }
    }
    
    TaskLib::TaskManager taskManager;
    
    auto countdownID = taskManager.createTask([](){
        std::cout << std::endl;
        int i = 10;
        while(i > 0) {
            std::cout << i << std::endl;
            i--;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }, [](){ std::cout << "Liftoff !" << std::endl; });
    
    auto rocketLaunchID = taskManager.createTask([&](){
        std::cout << "Initiating countdown..." << std::endl;
        taskManager.start(countdownID);
    }, [](){ std::cout << "To infinity and beyond !" << std::endl; });
    
    auto infiniteLoopID = taskManager.createTask([](){
        while(true) {}
    });
    
    bool canContinue = true;
    std::unordered_map<std::string, std::function<void (int)>> actions = {
            {"help", [&](int i){ displayHelp(); }},
            {"quit", [&](int i){ canContinue = false; std::cout << "Quitting..." << std::endl; taskManager.stopAllTasks(); }},
            
            {"start", [&](int i){ if(taskManager.start(rocketLaunchID)) std::cout << rocketLaunchID << std::endl; }},
            {"pause", [&](TaskLib::TaskID id){ if(taskManager.pause(rocketLaunchID)) std::cout << "Paused " << id << std::endl; }},
            {"resume", [&](TaskLib::TaskID id){ if(taskManager.resume(rocketLaunchID)) std::cout << "Restarted " << id << std::endl; }},
            {"stop", [&](TaskLib::TaskID id){ if(taskManager.stop(rocketLaunchID)) std::cout << "Stopped " << id << std::endl; }},
            {"status", [&](TaskLib::TaskID id = -1){
                if(id < 0) {
                    taskManager.printAllStatuses();
                }
                else if(taskManager.statusOfTask(id)) std::cout << id << std::endl;
            }},

            {"startAll", [&](int i){taskManager.startAllTasks();}},
            {"pauseAll", [&](int i){taskManager.pauseAllTasks();}},
            {"resumeAll", [&](int i){taskManager.resumeAllTasks();}},
            {"stopAll", [&](int i){taskManager.stopAllTasks();}},
    };
    
    std::string command;
    while(canContinue) {
        std::cout << "Enter a command : ";
        std::cin >> command;
        std::cout << std::endl;
        
        std::istringstream iss(command);
        std::vector<std::string> args(std::istream_iterator<std::string>{iss},
                                      std::istream_iterator<std::string>());
        int input = -1;
        if(args.size() > 1) input = std::stoi(args[1]);
        
        if(actions.find(command) != actions.end()) actions.at(command)(input);
        else std::cout << "Unavailable command" << std::endl;
    }
    
    return 0;
}