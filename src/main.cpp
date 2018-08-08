
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

class TaskTester {
public:
    void hello() { std::cout << "Hello from example" << std::endl; }
    void printMembers() { std::cout << "i: " << m_i << ", j: " << m_j << std::endl; }
    template<typename T>
    void print(T in) { std:: cout << in << std::endl;}
    
    int getI() { return m_i; }
    float getJ() { return m_j; }

private:
    int m_i = 42;
    float m_j = 3.14;
};

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
    
    
    std::string fib = "112358";
    std::atomic<int> m = 0;
    TaskTester tester;
    taskManager.createTask([&](){tester.hello();});
    taskManager.createTask([&](){tester.printMembers();});
    taskManager.createTask([&](){tester.print(fib);});
    auto id = taskManager.createTask([&](){ m.store(tester.getI()); });
    std::cout << m.load() << std::endl;
    taskManager.start(id);
    
    
    auto countdownID = taskManager.createTask([](){
        std::cout << std::endl;
        int i = 10;
        while(i > 0) {
            std::cout << i << std::endl;
            i--;
            // std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }, [](){ std::cout << "Liftoff !" << std::endl; });
    
    auto rocketLaunchID = taskManager.createTask([&](){
        std::cout << "\nAll systems are go...\nInitiating countdown" << std::endl;
    }, [&](){ taskManager.start(countdownID); });
    
    auto infiniteLoopID = taskManager.createTask([](){
        while(true) {}
    });
    
    bool canContinue = true;
    std::unordered_map<std::string, std::function<void (int)>> actions = {
            {"", [](int i){std::cout << std::endl;}},
            {"help", [&](int i){ displayHelp(); }},
            {"quit", [&](int i){ canContinue = false; std::cout << "Quitting..." << std::endl; taskManager.stopAllTasks(); }},
            
            {"start", [&](TaskLib::TaskID id){
                if (id < 0) id = rocketLaunchID;
                if(taskManager.start(id)) std::cout << id << std::endl;
            }},
            {"pause", [&](TaskLib::TaskID id){ if(taskManager.pause(id)) std::cout << "Paused " << id << std::endl; }},
            {"resume", [&](TaskLib::TaskID id){ if(taskManager.resume(id)) std::cout << "Restarted " << id << std::endl; }},
            {"stop", [&](TaskLib::TaskID id){ if(taskManager.stop(id)) std::cout << "Stopped " << id << std::endl; }},
            {"status", [&](TaskLib::TaskID id = -1){
                if(id < 0) {
                    taskManager.allStatuses();
                }
                else if(taskManager.status(id)) std::cout << id << std::endl;
            }},

            {"startAll", [&](int i){taskManager.startAllTasks();}},
            {"pauseAll", [&](int i){taskManager.pauseAllTasks();}},
            {"resumeAll", [&](int i){taskManager.resumeAllTasks();}},
            {"stopAll", [&](int i){taskManager.stopAllTasks();}},
    };
    
    std::string command;
    while(canContinue) {
        std::cout << "> ";
        std::getline(std::cin, command);
        std::cout << std::endl;
        
        std::istringstream iss(command);
        std::vector<std::string> args(std::istream_iterator<std::string>{iss},
                                      std::istream_iterator<std::string>());
        int input = -1;
        if(args.size() > 1) {
            input = std::stoi(args[1]);
            command = args[0];
        }
        
        if(actions.find(command) != actions.end()) actions.at(command)(input);
        else std::cout << "Unavailable command" << std::endl;
    }
    
    return 0;
}