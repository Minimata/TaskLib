//
// Created by Minimata on 05.08.2018.
//

#pragma once

#include <functional>
#include <iostream>
#include <thread>
// #include "../thread/mingw.thread.h"

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
        Task(TaskID id, F&& func) : m_taskID(id), m_state(STARTING_STATE), m_taskToExecute(func) {}
        
        ~Task() = default;
        
        TaskID getID() {
            return m_taskID;
        }
        State getState() {
            return m_state;
        }
        
        void start() {
        }
        void pause() {}
        void resume() {}
        void stop() {}
    
    private:
        TaskID m_taskID;
        State m_state;
        std::function<void()> m_taskToExecute;  /// constraining
    };
}


