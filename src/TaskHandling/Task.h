//
// Created by Minimata on 05.08.2018.
//

#pragma once

#include <functional>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "../../thread/mingw.thread.h"
#include "../../thread/mingw.mutex.h"
#include "../Utils/CPP11Helpers.h"

#endif

namespace TaskLib {
    
    using TaskID = int;
    
    enum State {
        paused,
        running,
        stopped,
        completed
    };
    const State STARTING_STATE = paused;
    const std::string stateNames[] = {
            "paused",
            "running",
            "stopped",
            "completed"
    };
    
    void nothingFunction() {}
    
    
    class Task {
    
    public:
        Task(TaskID id) : Task(id, nothingFunction, nothingFunction) {}
        template<typename F>
        Task(TaskID id, F&& func) : Task(id, func, nothingFunction) {}
        template <typename F, typename C>
        Task(TaskID id, F&& func, C&& callback) :
                m_taskID(id),
                m_state(STARTING_STATE),
                m_asyncTask(func),
                m_callback(callback),
                m_asyncTaskThread(),
                m_threadCompleted()
            {}
        Task(const Task& other) : m_mutex()  {
            m_taskID = other.m_taskID;
            m_asyncTask = other.m_asyncTask;
            
            m_state = paused;
            m_asyncTaskThread = std::thread();
            m_threadCompleted = std::thread();
        }
        ~Task() {
            if(m_asyncTaskThread.joinable()) m_asyncTaskThread.join();
            if(m_threadCompleted.joinable()) m_threadCompleted.join();
        }
        
        Task& operator=(Task other) {
            std::swap(m_taskID, other.m_taskID);
            std::swap(m_asyncTask, other.m_asyncTask);
            
            m_state = STARTING_STATE;
            m_asyncTaskThread = std::thread();
            m_threadCompleted = std::thread();
        }
        
        TaskID getID() { return m_taskID; }
        State getState() { return m_state; }
        
        template <typename F>
        void setAsyncTask(F func) { m_asyncTask = func; }
        template <typename C>
        void setCallback(C cb) { m_callback = cb; }
        
        void start() {
            if(m_state == paused) {
                m_asyncTaskThread = std::thread(m_asyncTask);
                m_threadCompleted = std::thread([this]() {
                    if (m_asyncTaskThread.joinable()) m_asyncTaskThread.join();
                    atomic_setState(completed);
                    m_callback();
                });
                atomic_setState(running);
            }
        }
        void stop() {
            if(m_state==running || m_state==paused) {
                if(m_asyncTaskThread.joinable()) m_asyncTaskThread.detach();
                if(m_threadCompleted.joinable()) m_threadCompleted.detach();
                atomic_setState(stopped);
            }
        }
        /**
         * From what I read, it's impossible and not wanted to pause and resume a thread in which we have no access.
         * If the user wants to pause and resume his threads from another thread, he must use condition variables or
         * future / promises.
         * In my case, since I take any function as an argument to give to a thread, I have no way to implement this
         * pause / resume mechanism without executing all of the function at once.
         * Hence, the "pause / resume" mechanic is just a stop with a restart.
         */
        void pause() {
            if(m_state == running) {
                stop();
                atomic_setState(paused);
            }
        }
        void resume() {
            start();
        }
        void join() {
            if (m_asyncTaskThread.joinable()) m_asyncTaskThread.join();
        }
    
    private:
        void atomic_setState(State s) {
            m_mutex.lock();
            m_state = s;
            m_mutex.unlock();
        }
        
        TaskID m_taskID;
        State m_state;
        std::function<void()> m_asyncTask;
        std::function<void()> m_callback;
        
        std::thread m_asyncTaskThread;
        std::thread m_threadCompleted;
        std::mutex m_mutex;
    };
}


