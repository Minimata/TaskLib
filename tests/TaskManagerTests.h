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

TEST(CreateTaskTest, CompatibleFunctions) {
    TaskLib::TaskManager taskManager;
    
    // With lambda
    int i = 42;
    // EXPECT_GT(-1, taskManager.createTask([i](){return i;}));
    
    // With input and output parameters
    EXPECT_GT(-1, taskManager.createTask(NothingFunction));
    //EXPECT_GT(-1, taskManager.createTask(InputFunction));
    //EXPECT_GT(-1, taskManager.createTask(ReturnFunction));
    //EXPECT_GT(-1, taskManager.createTask(Identity));
}
