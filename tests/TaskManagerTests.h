//
// Created by Minimata on 05.08.2018.
//

#pragma once


#include "gtest/gtest.h"
#include "../src/TaskManager.h"

#include <chrono>
#include <thread>
#include "../thread/mingw.thread.h"

void NothingFunction() { }

class TaskManagerFixture: public ::testing::Test {
protected:
    // put in any custom data members that you need
    TaskLib::TaskManager taskManager;
    TaskLib::TaskID taskID;
    TaskLib::TaskID quickTaskID;
    
    TaskManagerFixture() : taskManager() {
        taskID = taskManager.createTask([](){
            std::cout << "Subroutine started." << std::endl;
            int i = INT32_MAX;
            while(i > 0) i--;
            std::cout << "Subroutine ended." << std::endl;
        });
        quickTaskID = taskManager.createTask([](){});
    }
    ~TaskManagerFixture( )  {
        // cleanup any pending stuff, but no exceptions allowed
    }
    
    void SetUp( ) {
        // code here will execute just before the test ensues
    }
    void TearDown( ) {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }
    
    TaskLib::State startTasks() {
        taskManager.startAllTasks();
        return taskManager.statusOfTask(taskID);
    }
    TaskLib::State pauseTasks() {
        taskManager.startAllTasks();
        taskManager.pauseAllTasks();
        return taskManager.statusOfTask(taskID);
    }
    TaskLib::State resumeTasks() {
        taskManager.startAllTasks();
        taskManager.pauseAllTasks();
        taskManager.resumeAllTasks();
        return taskManager.statusOfTask(taskID);
    }
    TaskLib::State stopTasks() {
        taskManager.startAllTasks();
        taskManager.stopAllTasks();
        return taskManager.statusOfTask(taskID);
    }
    TaskLib::State taskCompleted() {
        taskManager.startAllTasks();
        std::thread x;
        std::this_thread::sleep_for(std::chrono::nanoseconds(10));
        return taskManager.statusOfTask(quickTaskID);
    }
    
};

TEST_F(TaskManagerFixture, CreateTask) {
    EXPECT_LE(0, taskManager.createTask(NothingFunction));
}

TEST_F(TaskManagerFixture, TaskStates) {
    EXPECT_EQ(TaskLib::paused, taskManager.statusOfTask(taskID));
    EXPECT_EQ(TaskLib::running, startTasks());
    EXPECT_EQ(TaskLib::paused, pauseTasks());
    EXPECT_EQ(TaskLib::running, resumeTasks());
    EXPECT_EQ(TaskLib::stopped, stopTasks());
    EXPECT_EQ(TaskLib::completed, taskCompleted());
}

