
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
    std::cout   << "\nWelcome to the TaskLib example program !\n\n"
                
                << "This example application lets you fiddle with some of the features of this asynchronous task management library.\n"
                << "A few simple tasks have been set up for you to test, one of them being an infinite loop.\n"
                << "With the program running, type the following commands in your terminal to manipulate the tasks.\n\n\n"
                
                
                << "\t - help : displays this message.\n"
                << "\t - quit : detach threads still running and shuts down program.\n\n"
                
                << "\t - status : prints the status of all the tasks.\n"
                << "\t - status [integer] : prints the status of the task with the given ID.\n\n"
                
                << "\t - start : launches a rocket!\n"
                << "\t - start [integer] : starts the task with the given ID.\n"
                << "\t - start [string] : starts all the tasks of the given type.\n\n"
                
                << "\t - pause [integer] : pauses the task with the given ID.\n\n"
                
                << "\t - resume [integer] : resumes (or restarts) the task with the given ID.\n\n"
                
                << "\t - stop [integer] : stops the task with the given ID.\n\n"
                << std::endl;
}


class TaskTester {
public:
    TaskTester() : m_i(42), m_j(3.14) {}
    
    void hello() { std::cout << "Hello from example" << std::endl; }
    void printMembers() { std::cout << "i: " << m_i << ", j: " << m_j << std::endl; }
    template<typename T>
    void print(T in) { std:: cout << in << std::endl;}
    
    int getI() { return m_i; }
    float getJ() { return m_j; }
    
    virtual int compare() const { return 42; }
    friend std::ostream& operator<<(std::ostream& os, const TaskTester& t) {
        os << 42;
        return os;
    }

protected:
    int m_i;
    float m_j;
};

class TaskChildTester : public TaskTester {
public:
    int compare() const override { return 64; }
    friend std::ostream& operator<<(std::ostream& os, const TaskChildTester& t) {
        os << 64;
        return os;
    }
};


int main(int argc, char** argv) {
    
    // In case of argument
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
    
    /**
    std::string fib = "112358";
    std::atomic<int> m(0);
    TaskTester tester;
    
    taskManager.createTask([&](){tester.hello();});
    taskManager.createTask([&](){tester.printMembers();});
    taskManager.createTask([&](){tester.print(fib);});
    
    auto returnValueID = taskManager.createTask([&](){ m.store(tester.getI()); } );
    std::cout << m << std::endl;
    taskManager.start(returnValueID);
    taskManager.join(returnValueID);
    std::cout << m << std::endl;
    
    auto emptyTaskID = taskManager.createTask();
    taskManager.setAsyncTask(emptyTaskID, [](){ std::cout << "Not so empty anymore... ";});
    taskManager.setCallback(emptyTaskID, [](){ std::cout << "Is it ?" << std::endl; });
    taskManager.start(emptyTaskID);
    
    auto onStartID = taskManager.createTask();
    taskManager.start(onStartID,
            [](){ std::cout << "Function set on start... ";},
            [](){ std::cout << "And callback too!" << std::endl; } );
    
    taskManager.startAllTasks();
    taskManager.joinAllTasks();
    taskManager.removeAllTasks();
    
    
    TaskLib::Task t1(1);
    TaskLib::Task t2(2);
    t1.setType(std::string("hello"));
    t2.setType(std::string("goodbye"));
    
    if( t1.compareType(std::string("hello"), [](std::string s1, std::string s2){ return s1 == s2; }) )
        std::cout << "Should show" << std::endl;
    if( t2.compareType(std::string("hello"), [](std::string s1, std::string s2){ return s1 == s2; }) )
        std::cout << "Should not show" << std::endl;
    
    auto id = taskManager.createTask([](){std::cout << "Task1" << std::endl;});
    taskManager.setType(id, TaskTester());
    id = taskManager.createTask([](){std::cout << "Task2" << std::endl;});
    taskManager.setType(id, TaskTester());
    id = taskManager.createTask([](){std::cout << "Task3" << std::endl;});
    taskManager.setType(id, TaskChildTester());
     
     */
    
    auto rocketLaunchID = taskManager.createTask([&](){
        std::cout << "\nAll systems are go...\nLiftoff in 30 seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(20));
        std::cout << "10 seconds left to liftoff..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }, [&](){ std::cout << "Liftoff ! To infinity and beyond !" << std::endl; });
    
    taskManager.createTask([](){std::cout << "First task of type 'Hello'" << std::endl; }, [](){}, std::string("Hello"));
    taskManager.createTask([](){std::cout << "Second task of type 'Hello'" << std::endl; }, [](){}, std::string("Hello"));
    taskManager.createTask([](){std::cout << "Only task of type 'world!'" << std::endl; }, [](){}, std::string("world!"));
    
    auto infiniteLoopID = taskManager.createTask([](){
        while(true) {}
    });
    
    bool canContinue = true;
    std::unordered_map<std::string, std::function<void (int)>> actions = {
            {"", [](int){std::cout << std::endl;}},
            {"help", [&](int){ displayHelp(); }},
            {"quit", [&](int){
                canContinue = false;
                std::cout << "Quitting..." << std::endl;
                taskManager.stopAllTasks();
            }},
            
            {"start", [&](TaskLib::TaskID id){
                if (id < 0) id = rocketLaunchID;
                try {
                    taskManager.start(id); std::cout << id << std::endl;
                }
                catch(const std::runtime_error& e) {
                    std::cerr << e.what() << std::endl;
                }
            }},
            {"pause", [&](TaskLib::TaskID id){
                try {
                    taskManager.pause(id); std::cout << "Paused " << id << std::endl;
                }
                catch(const std::runtime_error& e) {
                    std::cerr << e.what() << std::endl;
                }
            }},
            {"resume", [&](TaskLib::TaskID id){
                try {
                    taskManager.resume(id); std::cout << "Resume " << id << std::endl;
                }
                catch(const std::runtime_error& e) {
                    std::cerr << e.what() << std::endl;
                }
            }},
            {"stop", [&](TaskLib::TaskID id){
                try {
                    taskManager.stop(id); std::cout << "Resume " << id << std::endl;
                }
                catch(const std::runtime_error& e) {
                    std::cerr << e.what() << std::endl;
                }
            }},
            {"join", [&](TaskLib::TaskID id){
                try {
                    taskManager.join(id); std::cout << "Waiting on " << id << std::endl;
                }
                catch(const std::runtime_error& e) {
                    std::cerr << e.what() << std::endl;
                }
            }},
            {"status", [&](TaskLib::TaskID id = -1){
                if(id < 0) {
                    taskManager.statusAllTasks();
                }
                else {
                    try {
                        taskManager.status(id);
                    }
                    catch (const std::runtime_error& e) {
                        std::cerr << e.what() << std::endl;
                    }
                }
            }}
    };
    
    std::string command;
    while(canContinue) {
        // Waits on command and retrieves it
        std::cout << "> ";
        std::getline(std::cin, command);
        std::cout << std::endl;
        
        // Separate command from its arguments
        std::istringstream iss(command);
        std::vector<std::string> args(std::istream_iterator<std::string>{iss},
                                      std::istream_iterator<std::string>());
        
        // Sets input, if any
        int input = -1;
        if(args.size() > 1) {
            command = std::string(args[0]);
            try {
                input = std::stoi(args[1]);
            }
            catch(const std::invalid_argument& e) {
                if(command == std::string("start"))
                    taskManager.startTaskOfType(std::string(args[1]), [](std::string s1, std::string s2){ return s1 == s2; });
                else {
                    std::cout << "Invalid command argument" << std::endl;
                }
                continue;
            }
        }
        
        // Launch command, if valid
        if(actions.find(command) != actions.end()) actions.at(command)(input);
        else std::cout << "Unavailable command" << std::endl;
    }
    
    return 0;
}