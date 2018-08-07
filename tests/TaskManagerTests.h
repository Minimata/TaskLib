//
// Created by Minimata on 05.08.2018.
//

#pragma once


#include "gtest/gtest.h"
#include "../src/TaskHandling/TaskManager.h"

#include <chrono>
#include <thread>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "../thread/mingw.thread.h"
#endif


namespace TaskLib {
    namespace Tests {
        
        class TaskManagerFixture : public ::testing::Test {
        protected:
            
            TaskManagerFixture() : m_taskManager() {
            
            }
            
            ~TaskManagerFixture() = default;  // cleanup any pending stuff, but no exceptions allowed
    
    
            void SetUp() {
                // code here will execute just before the test ensues
            }
            
            void TearDown() {
                // code here will be called just after the test completes
                // ok to through exceptions from here if need be
            }
    
            // put in any custom data members that you need
            TaskManager m_taskManager;
        };
        
        TEST_F(TaskManagerFixture, ValidCreateTask) {
            EXPECT_LE(0, m_taskManager.createTask());
            EXPECT_LE(0, m_taskManager.createTask([](){}));
            EXPECT_LE(0, m_taskManager.createTask([](){}, [](){}));
        }
    }
}
