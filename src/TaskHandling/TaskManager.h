//
// Created by Minimata on 05.08.2018.
//

#pragma once

#include "Task.h"
#include "CPP11Helpers.h"

#include <memory>
#include <unordered_map>
#include <utility>

#include <cstdlib>
#include <ctime>
#include <sstream>

namespace TaskLib {
    
    const int MAX_NUMBER_OF_TASKS = 4096;
    
    class TaskManager {
    
    public:
        TaskManager() {
            srand (time(nullptr));
        }
        ~TaskManager() = default;
        
        TaskID createTask() {
            return createTask([](){}, [](){}, DEFAULT_TYPE);
        }
        template<typename F>
        TaskID createTask(F&& func) {
            return createTask(func, [](){}, DEFAULT_TYPE);
        }
        template <typename F, typename C>
        TaskID createTask(F&& func, C&& callback) {
            return createTask(func, callback, DEFAULT_TYPE);
        }
        template <typename F, typename C, typename T>
        TaskID createTask(F&& func, C&& callback, T type) {
            // Generate a random and unique ID small enough to be comfortable to type
            auto taskID = TaskID{rand() % MAX_NUMBER_OF_TASKS};
            while(m_tasks.find(taskID) != m_tasks.end()) taskID = TaskID{rand()};
        
            // Store the task in the map
            m_tasks.emplace(std::make_pair(taskID, CPP11Helpers::make_unique<Task>(taskID, func, callback, std::move(type))));
            return taskID;
        }
        
        void start(const TaskID& id) {
            execute([&id, this](){ m_tasks.at(id)->start(); }, id);
        }
        template <typename F>
        void start(const TaskID& id, F&& func) {
            execute([&, this](){ auto task = m_tasks.at(id); task->setAsyncTask(func); task->start(); }, id);
        }
        template <typename F, typename C>
        void start(const TaskID& id, F&& func, C&& cb) {
            execute([&, this](){ auto task = m_tasks.at(id); task->setAsyncTask(func); task->setCallback(cb); task->start(); }, id);
        }
        
        void pause(const TaskID& id) {
            execute([&id, this](){ m_tasks.at(id)->pause(); }, id);
        }
        void resume(const TaskID& id) {
            execute([&id, this](){ m_tasks.at(id)->resume(); }, id);
        }
        void stop(const TaskID& id) {
            execute([&id, this](){ m_tasks.at(id)->stop(); }, id);
        }
        void status(const TaskID& id) {
            execute([&id, this](){ m_tasks.at(id)->print(); }, id);
        }
        void join(const TaskID& id) {
            execute([&id, this](){ m_tasks.at(id)->join(); }, id);
        }
        void removeTask(const TaskID& id) {
            execute([&id, this](){ stop(id); m_tasks.erase(id); }, id);
        }
    
        template<typename F>
        void setAsyncTask(const TaskID& id, F&& func) {
            execute([&, this](){ m_tasks.at(id)->setAsyncTask(func); }, id);
        }
        template<typename F>
        void setCallback(const TaskID& id, F&& func) {
            execute([&, this](){ m_tasks.at(id)->setCallback(func); }, id);
        }
        template<typename T>
        void setType(const TaskID& id, T type) {
            execute([&, this](){ m_tasks.at(id)->setType(type); }, id);
        }
    
        template <typename T>
        void startTaskOfType(T type) {
            startTaskOfType(type, [](T t1, T t2){ return t1 == t2; });
        }
        template <typename T, typename C>
        void startTaskOfType(T type, C&& compareFunction) {
            match(type, [this](std::unique_ptr<Task>& t){ t->start(); }, compareFunction);
        }
    
        template <typename T>
        void pauseTaskOfType(T type) {
            pauseTaskOfType(type, [](T t1, T t2){ return t1 == t2; });
        }
        template <typename T, typename C>
        void pauseTaskOfType(T type, C&& compareFunction) {
            match(type, [this](std::unique_ptr<Task>& t){ t->pause(); }, compareFunction);
        }
    
        template <typename T>
        void resumeTaskOfType(T type) {
            resumeTaskOfType(type, [](T t1, T t2){ return t1 == t2; });
        }
        template <typename T, typename C>
        void resumeTaskOfType(T type, C&& compareFunction) {
            match(type, [this](std::unique_ptr<Task>& t){ t->resume(); }, compareFunction);
        }
        
        template <typename T>
        void stopTaskOfType(T type) {
            stopTaskOfType(type, [](T t1, T t2){ return t1 == t2; });
        }
        template <typename T, typename C>
        void stopTaskOfType(T type, C&& compareFunction) {
            match(type, [this](std::unique_ptr<Task>& t){ t->stop(); }, compareFunction);
        }
    
        template <typename T>
        void joinTaskOfType(T type) {
            joinTaskOfType(type, [](T t1, T t2){ return t1 == t2; });
        }
        template <typename T, typename C>
        void joinTaskOfType(T type, C&& compareFunction) {
            match(type, [this](std::unique_ptr<Task>& t){ t->join(); }, compareFunction);
        }
    
        template <typename T>
        void statusTaskOfType(T type) {
            statusTaskOfType(type, [](T t1, T t2){ return t1 == t2; });
        }
        template <typename T, typename C>
        void statusTaskOfType(T type, C&& compareFunction) {
            match(type, [this](std::unique_ptr<Task>& t){ t->print(); }, compareFunction);
        }
        
        void startAllTasks() {
            for (const auto& task : m_tasks) task.second->start();
        }
        void pauseAllTasks() {
            for (const auto& task : m_tasks) task.second->pause();
        }
        void resumeAllTasks() {
            for (const auto& task : m_tasks) task.second->resume();
        }
        void stopAllTasks() {
            for (const auto& task : m_tasks) task.second->stop();
        }
        void statusAllTasks() {
            for (const auto& task : m_tasks) task.second->print();
        }
        void joinAllTasks() {
            for (const auto& task : m_tasks) task.second->join();
        }
        void removeAllTasks() {
            stopAllTasks();
            m_tasks.clear();
        }
        
    
    private:
    
        template <typename F>
        void execute(F&& func, const TaskID& id) {
            if(m_tasks.find(id) != m_tasks.end())
                func();
            else {
                std::stringstream sstream;
                sstream << "No task with id " << id;
                throw std::runtime_error(sstream.str());
            }
        }
        
        template <typename T, typename F, typename C>
        void match(T type, F&& func, C&& compare) {
            for(auto& task : m_tasks) {
                if(task.second->compareType(type, compare)) {
                    func(task.second);
                }
            }
        }
        
        std::unordered_map<TaskID, std::unique_ptr<Task>> m_tasks;
    };
}

