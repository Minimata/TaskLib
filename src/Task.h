//
// Created by Minimata on 05.08.2018.
//

#pragma once

#include <functional>
#include <iostream>

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
        
        void start() {std::cout << "start task " << m_taskID << std::endl; m_state = running;}
        void pause() {std::cout << "pause task " << m_taskID << std::endl; m_state = paused;}
        void resume() {std::cout << "resume task " << m_taskID << std::endl; m_state = running;}
        void stop() {std::cout << "stop task " << m_taskID << std::endl; m_state = stopped;}
    
    private:
        TaskID m_taskID;
        State m_state;
        std::function<void()> m_taskToExecute;  /// constraining
    };
}


