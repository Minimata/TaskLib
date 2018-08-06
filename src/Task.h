//
// Created by Minimata on 05.08.2018.
//

#pragma once

#include <functional>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "../thread/mingw.thread.h"
#include "../thread/mingw.condition_variable.h"
#include "../thread/mingw.mutex.h"
// #include "../thread/mingw.shared_mutex.h"

namespace TaskLib {
    
    using TaskID = int;
    
    enum State {
        paused,
        running,
        stopped,
        completed
    };
    const State STARTING_STATE = paused;
    
    class Task {
    
    public:
        template<typename F>
        Task(TaskID id, F&& func) :
            m_taskID(id),
            m_state(STARTING_STATE),
            m_taskToExecute(func),
            m_asyncTask(),
            m_threadCompleted()
            {}
        
        ~Task() {
            if(m_asyncTask.joinable()) m_asyncTask.join();
            if(m_threadCompleted.joinable()) m_threadCompleted.join();
        }
        
        TaskID getID() {
            return m_taskID;
        }
        State getState() {
            return m_state;
        }
        
        void start() {
            m_asyncTask = std::thread(m_taskToExecute);
            m_threadCompleted = std::thread([this]() {
                m_asyncTask.join();
                atomic_setState(completed);
            });
            atomic_setState(running);
        }
        void pause() {
            atomic_setState(paused);
        }
        void resume() {
            atomic_setState(running);
        }
        void stop() {
            atomic_setState(stopped);
            m_asyncTask.detach();
            m_threadCompleted.detach();
        }
    
    private:
        void atomic_setState(State s) {
            m_mutex.lock();
            m_state = s;
            m_mutex.unlock();
        }
        
        TaskID m_taskID;
        State m_state;
        std::function<void()> m_taskToExecute;  /// constraining
        
        std::thread m_asyncTask;
        std::thread m_threadCompleted;
        std::mutex m_mutex;
        std::condition_variable conditionVariable;
    };
}


