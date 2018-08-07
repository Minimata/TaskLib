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
        
        void NothingFunction() {}
        
        class TaskManagerFixture : public ::testing::Test {
        protected:
            // put in any custom data members that you need
            TaskManager m_taskManager;
            TaskID m_taskID;
            TaskID m_quickTaskID;
            
            TaskManagerFixture() : m_taskManager() {
                m_taskID = m_taskManager.createTask( []() {
                    std::cout << "Subroutine started." << std::endl;
                    int i = 20;
                    while (i > 0) i--;
                    std::cout << "Subroutine ended." << std::endl;
                } );
                m_quickTaskID = m_taskManager.createTask( [](){} );
            }
            
            ~TaskManagerFixture() = default;  // cleanup any pending stuff, but no exceptions allowed
    
    
            void SetUp() {
                // code here will execute just before the test ensues
            }
            
            void TearDown() {
                // code here will be called just after the test completes
                // ok to through exceptions from here if need be
            }
            
            State startTasks() {
                m_taskManager.startAllTasks();
                return m_taskManager.statusOfTask(m_taskID);
            }
            
            State pauseTasks() {
                m_taskManager.startAllTasks();
                m_taskManager.pauseAllTasks();
                return m_taskManager.statusOfTask(m_taskID);
            }
            
            State resumeTasks() {
                m_taskManager.startAllTasks();
                m_taskManager.pauseAllTasks();
                m_taskManager.resumeAllTasks();
                return m_taskManager.statusOfTask(m_taskID);
            }
            
            State stopTasks() {
                m_taskManager.startAllTasks();
                m_taskManager.stopAllTasks();
                return m_taskManager.statusOfTask(m_taskID);
            }
            
            State taskCompleted() {
                m_taskManager.startAllTasks();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                return m_taskManager.statusOfTask(m_quickTaskID);
            }
            
        };
        
        TEST_F(TaskManagerFixture, ValidCreateTask) {
            EXPECT_LE(0, m_taskManager.createTask(NothingFunction));
        }
        
        /**
        TEST_F(TaskManagerFixture, ValidTaskStates) {
            EXPECT_EQ(paused, m_taskManager.statusOfTask(m_taskID));
            EXPECT_EQ(running, startTasks());
            EXPECT_EQ(paused, pauseTasks());
            EXPECT_EQ(running, resumeTasks());
            EXPECT_EQ(stopped, stopTasks());
            EXPECT_EQ(completed, taskCompleted());
        }
         */
        
    }
}
