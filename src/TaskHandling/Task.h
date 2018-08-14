//
// Created by Minimata on 05.08.2018.
//

#pragma once

#include <functional>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include "CPP11Helpers.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "../../thread/mingw.thread.h"
#include "../../thread/mingw.mutex.h"

#endif

namespace TaskLib {
    
    using TaskID = int;
    const auto DEFAULT_TYPE = std::string{"default"};
    
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
        Task(TaskID id) : Task(id, nothingFunction, nothingFunction, DEFAULT_TYPE) {}
        template<typename F>
        Task(TaskID id, F&& func) : Task(id, func, nothingFunction, DEFAULT_TYPE) {}
        template <typename F, typename C>
        Task(TaskID id, F&& func, C&& callback) : Task(id, func, callback, DEFAULT_TYPE) {}
        template <typename F, typename C, typename T>
        Task(TaskID id, F&& func, C&& callback, T type) :
                m_taskID(id),
                m_state(STARTING_STATE),
                m_asyncTask(func),
                m_callback(callback),
                m_asyncTaskThread(),
                m_threadCompleted()
        { setType(std::move(type)); }
        ~Task() {
            if(m_asyncTaskThread.joinable()) m_asyncTaskThread.join();
            if(m_threadCompleted.joinable()) m_threadCompleted.join();
        }
        
        TaskID getID() { return m_taskID; }
        State getState() { return m_state; }
        
        template <typename F>
        void setAsyncTask(F&& func) { m_asyncTask = func; }
        template <typename C>
        void setCallback(C&& cb) { m_callback = cb; }
        
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
        void print() {
            std::cout << m_taskID << ": " << stateToString(m_state)  << " \t- type: ";
            m_taskType->print(std::cout);
            std::cout << std::endl;
        }
        
        template <typename T>
        void setType(T type) { m_taskType = CPP11Helpers::make_unique<Type<T>>(std::move(type)); }
    
    
        template <typename T>
        bool compareType(T comparisonType) {
            return compareType(comparisonType, [](T t1, T t2){ return t1 == t2; });
        }
        template <typename T, typename F>
        bool compareType(T comparisonType, F&& compareFunction) {
            auto downcastType = static_cast<Type<T>*>(m_taskType.get());
            return compareTypes<T>(downcastType, comparisonType, compareFunction);
        };
    
    private:
        template <typename Type, typename T1, typename F>
        bool compareTypes(T1 taskType, Type other, F&& func) {
            auto t1 = static_cast<Type>(taskType->m_type);
            return func(t1, other);
        }
        
        std::string stateToString(State s) { return stateNames[s]; }
        
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
    
    
        struct ConceptType {
            virtual ~ConceptType() = default;
            virtual void print(std::ostream&) = 0;
        };
        
        template <typename T>
        struct Type final : ConceptType {
            explicit Type(T x) : m_type(std::move(x)) {}
            void print(std::ostream& os) override { os << m_type; }
            T m_type;
        };
        
        std::unique_ptr<ConceptType> m_taskType;
    };
}


