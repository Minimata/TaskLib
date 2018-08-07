//
// Created by Minimata on 06.08.2018.
//

#pragma once

#include "gtest/gtest.h"
#include "../src/TaskHandling/Task.h"

#include <chrono>
#include <thread>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "../thread/mingw.thread.h"
#endif


namespace TaskLib {
    namespace Tests {
        
        class TaskFixture:  public ::testing::Test {
        protected:
            // put in any custom data members that you need
        
            TaskFixture() :
                m_task( 0, [](){ int i = INT32_MAX; while(i > 0) i--; } ),
                m_quickTask( 1, [](){} )
                {}
        
            ~TaskFixture() = default;  // cleanup any pending stuff, but no exceptions allowed
        
            void SetUp() {
                m_task = Task{ 0, [](){ int i = INT32_MAX; while(i > 0) i--; } };
                m_quickTask = Task{ 1, [](){} };
            }
            void TearDown() {
                // code here will be called just after the test completes
                // ok to through exceptions from here if need be
            }
            
            template <typename F>
            State quickTaskTest(F func) {
                m_quickTask.start();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                func();
                return m_task.getState();
            }
        
            Task m_task;
            Task m_quickTask;
        };
        
        TEST_F(TaskFixture, ValidStateChanges) {
            EXPECT_EQ(paused, m_task.getState());
            EXPECT_EQ(running, [this](){m_task.start(); return m_task.getState();}());
            EXPECT_EQ(paused, [this](){m_task.start(); m_task.pause(); return m_task.getState();}());
            EXPECT_EQ(running, [this](){
                m_task.start();
                m_task.pause();
                m_task.resume();
                return m_task.getState();
            }());
            EXPECT_EQ(stopped, [this](){m_task.start(); m_task.stop(); return m_task.getState();}());
            EXPECT_EQ(completed, quickTaskTest([](){}));
        }
        
        TEST_F(TaskFixture, InvalidStateChanges) {
            EXPECT_EQ(paused, [this](){m_task.stop(); return m_task.getState();}());
            EXPECT_EQ(paused, [this](){m_task.pause(); return m_task.getState();}());
            EXPECT_EQ(paused, [this](){m_task.resume(); return m_task.getState();}());
        
            EXPECT_EQ(running, [this](){m_task.start(); m_task.resume(); return m_task.getState();}());
            EXPECT_EQ(running, [this](){m_task.start(); m_task.start(); return m_task.getState();}());
        
            EXPECT_EQ(stopped, [this](){m_task.start(); m_task.stop(); m_task.start(); return m_task.getState();}());
            EXPECT_EQ(stopped, [this](){m_task.start(); m_task.stop(); m_task.pause(); return m_task.getState();}());
            EXPECT_EQ(stopped, [this](){m_task.start(); m_task.stop(); m_task.resume(); return m_task.getState();}());
        
            EXPECT_EQ(completed, quickTaskTest([this](){m_quickTask.start();}));
            EXPECT_EQ(completed, quickTaskTest([this](){m_quickTask.stop();}));
            EXPECT_EQ(completed, quickTaskTest([this](){m_quickTask.pause();}));
            EXPECT_EQ(completed, quickTaskTest([this](){m_quickTask.resume();}));
        }
        
    }
}


