//
// Created by Minimata on 05.08.2018.
//

#pragma once


#include "gtest/gtest.h"
#include "../src/TaskManager.h"

void NothingFunction() { }
void InputFunction(int i) { }
int ReturnFunction() {
    return 0;
}
float Identity(float i) {
    return i;
}

class TaskManagerFixture: public ::testing::Test {
protected:
    TaskManagerFixture() : taskManager() {
        // initialization code here
    }
    
    void SetUp( ) {
        // code here will execute just before the test ensues
    }
    
    void TearDown( ) {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }
    
    ~TaskManagerFixture( )  {
        // cleanup any pending stuff, but no exceptions allowed
    }
    
    // put in any custom data members that you need
    TaskLib::TaskManager taskManager;
};

TEST_F(TaskManagerFixture, CreateTask) {
    // With lambda
    // EXPECT_GT(-1, taskManager.createTask([i](){return i;}));
    
    // With input and output parameters
    EXPECT_LE(0, taskManager.createTask(NothingFunction));
    //EXPECT_LE(0, taskManager.createTask(InputFunction));
    //EXPECT_LE(0, taskManager.createTask(ReturnFunction));
    //EXPECT_LE(0, taskManager.createTask(Identity));
}

