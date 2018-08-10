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
        template <typename TaskID, typename TaskType, typename F, typename C>
        Task(TaskID x, TaskType t, F&& func, C&& cb) :
                m_self(CPP11Helpers::make_unique<TTask<TaskID, TaskType>>(
                    std::move(x),
                    std::move(t),
                    func,
                    cb) ) {}
        
        void start() { m_self->start(); }
        void stop() { m_self->stop(); }
        void pause() { m_self->pause(); }
        void resume() { m_self->resume(); }
        void join() { m_self->join(); }
    
    private:
    
        struct ConceptTask {
        public:
            virtual ~ConceptTask() = default;
            virtual void start() = 0;
            virtual void stop() = 0;
            virtual void pause() = 0;
            virtual void resume() = 0;
            virtual void join() = 0;
            
        private:
        
        };
        
        
        template <typename TaskID, typename TaskType>
        struct TTask final : ConceptTask {
            /**
            template <typename F, typename C>
            Task(TaskID x, TaskType t, F&& func, C&& cb) :
                m_taskID(std::move(x)),
                m_taskType(std::move(t)),
                m_asyncTask(func),
                m_callback(cb)
                {}
                */
        
        public:
            // TaskType is undefined
            explicit TTask(const TaskID& id) : TTask(id, TaskType(), nothingFunction, nothingFunction) {}
            template<typename F>
            TTask(const TaskID& id, F&& func) : TTask(id, TaskType(), func, nothingFunction) {}
            template <typename F, typename C>
            TTask(const TaskID& id, F&& func, C&& callback) : TTask(id, TaskType(), func, callback){}
            
            //TaskType is defined
            TTask(const TaskID& id, const TaskType& type) : TTask(id, type, nothingFunction, nothingFunction) {}
            template<typename F>
            TTask(const TaskID& id, const TaskType& type, F&& func) : TTask(id, type, func, nothingFunction) {}
            
            // Mother of all constructors
            template <typename F, typename C>
            TTask(const TaskID& id, const TaskType& type, F&& func, C&& callback) :
                    m_taskID(id),
                    m_taskType(type),
                    m_state(STARTING_STATE),
                    m_asyncTask(func),
                    m_callback(callback),
                    m_asyncTaskThread(),
                    m_threadCompleted()
            {}
    
            TTask(const TTask& other) : m_mutex()  {
                m_taskID = other.m_taskID;
                m_taskType = other.m_taskType;
                m_asyncTask = other.m_asyncTask;
                
                m_state = STARTING_STATE;
                m_asyncTaskThread = std::thread();
                m_threadCompleted = std::thread();
            }
            ~TTask() {
                if(m_asyncTaskThread.joinable()) m_asyncTaskThread.join();
                if(m_threadCompleted.joinable()) m_threadCompleted.join();
            }
            
            Task& operator=(TTask other) {
                std::swap(m_taskID, other.m_taskID);
                std::swap(m_asyncTask, other.m_asyncTask);
                
                m_state = STARTING_STATE;
                m_asyncTaskThread = std::thread();
                m_threadCompleted = std::thread();
            }
            
            TaskID getID() { return m_taskID; }
            State getState() { return m_state; }
            TaskType getType() { return m_taskType; }
            void setType(TaskType type) { m_taskType = type; }
            
            template <typename F>
            void setAsyncTask(F func) { m_asyncTask = func; }
            template <typename C>
            void setCallback(C cb) { m_callback = cb; }
            
            void start() override {
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
            void stop() override {
                if(m_state==running || m_state==paused) {
                    if(m_asyncTaskThread.joinable()) m_asyncTaskThread.detach();
                    if(m_threadCompleted.joinable()) m_threadCompleted.detach();
                    atomic_setState(stopped);
                }
            }
            void pause() override {
                if(m_state == running) {
                    stop();
                    atomic_setState(paused);
                }
            }
            void resume() override {
                start();
            }
            void join() override {
                if (m_asyncTaskThread.joinable()) m_asyncTaskThread.join();
            }
        
        private:
            void atomic_setState(State s) {
                m_mutex.lock();
                m_state = s;
                m_mutex.unlock();
            }
            
            TaskID m_taskID;
            TaskType m_taskType;
            std::function<void()> m_asyncTask;
            std::function<void()> m_callback;
        
            State m_state;
            std::thread m_asyncTaskThread;
            std::thread m_threadCompleted;
            std::mutex m_mutex;
        };
        
        std::unique_ptr<ConceptTask> m_self;
    
    };
}


