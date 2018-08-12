//
// Created by Minimata on 05.08.2018.
//

#pragma once

#include "Task.h"
#include "../Utils/CPP11Helpers.h"

#include <memory>
#include <unordered_map>
#include <utility>

#include <cstdlib>
#include <ctime>
#include <sstream>

namespace TaskLib {
    
    class TaskManager {
    
    public:
        TaskManager() {
            srand (time(nullptr));
        }
        ~TaskManager() = default;
        
        TaskID createTask() {
            return createTask([](){}, [](){});
        }
        template<typename F>
        TaskID createTask(F&& func) {
            return createTask(func, [](){});
        }
        template <typename F, typename C>
        TaskID createTask(F&& func, C&& callback) {
            // Generate a random and unique ID
            auto taskID = TaskID{rand()};
            while(m_tasks.find(taskID) != m_tasks.end()) taskID = TaskID{rand()};
    
            // Store the task right in the map
            m_tasks.emplace(std::make_pair(taskID, CPP11Helpers::make_unique<Task>(taskID, func, callback)));
            return taskID;
        }
        
        /**
         * TODO
         *
         * General:
         * Clean up the example main
         * Test what can be tested
         *
         * Features:
         * User defined Taskclasses and TaskID
         * Make the start method return a promise
         *
         */
        
        void start(const TaskID& id) {
            execute([&id, this](){startTask(m_tasks.at(id));}, id);
        }
        template <typename F>
        void start(const TaskID& id, F&& func) {
            execute([&, this](){ startTask(m_tasks.at(id), func); }, id);
        }
        template <typename F, typename C>
        void start(const TaskID& id, F&& func, C&& callback) {
            execute([&, this](){ startTask(m_tasks.at(id), func, callback); }, id);
        }
        
        void pause(const TaskID& id) {
            execute([&id, this](){pauseTask(m_tasks.at(id));}, id);
        }
        void resume(const TaskID& id) {
            execute([&id, this](){resumeTask(m_tasks.at(id));}, id);
        }
        void stop(const TaskID& id) {
            execute([&id, this](){stopTask(m_tasks.at(id));}, id);
        }
        void status(const TaskID& id) {
            execute([&id, this](){statusTask(m_tasks.at(id));}, id);
        }
        void join(const TaskID& id) {
            execute([&id, this](){joinTask(m_tasks.at(id));}, id);
        }
        void removeTask(const TaskID& id) {
            execute([&id, this](){ stop(id); m_tasks.erase(id); }, id);
        }
    
        template<typename F>
        void setAsyncTask(const TaskID& id, F&& func) {
            execute([&, this](){ setFunctionToTask(m_tasks.at(id), func); }, id);
        }
        template<typename F>
        void setCallback(const TaskID& id, F&& func) {
            execute([&, this](){ setCallbackToTask(m_tasks.at(id), func); }, id);
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
            for (const auto& task : m_tasks)
                std::cout << task.second->getID() << " : " << stateToString(task.second->getState()) << std::endl;
        }
        void joinAllTasks() {
            for (const auto& task : m_tasks) task.second->join();
        }
        void removeAllTasks() {
            stopAllTasks();
            m_tasks.clear();
        }
        
    
    private:
        std::string stateToString(const State& s) { return stateNames[s]; }
    
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
        void startTask(std::unique_ptr<Task>& task) {
            task->start();
        }
        template <typename F>
        void startTask(std::unique_ptr<Task>& task, F&& func) {
            task->setAsyncTask(func);
            task->start();
        }
        template <typename F, typename C>
        void startTask(std::unique_ptr<Task>& task, F&& func, C&& callback) {
            task->setAsyncTask(func);
            task->setCallback(callback);
            task->start();
        }
        void stopTask(std::unique_ptr<Task>& task) {
            task->stop();
        }
        void pauseTask(std::unique_ptr<Task>& task) {
            task->pause();
        }
        void resumeTask(std::unique_ptr<Task>& task) {
            task->resume();
        }
        void statusTask(std::unique_ptr<Task>& task) {
            std::cout << stateToString(task->getState()) << std::endl;
        }
        void joinTask(std::unique_ptr<Task>& task) {
            task->join();
        }
    
        template <typename F>
        void setFunctionToTask(std::unique_ptr<Task>& task, F&& func) {
            task->setAsyncTask(func);
        }
        template <typename F>
        void setCallbackToTask(std::unique_ptr<Task>& task, F&& func) {
            task->setCallback(func);
        }
        
        template <typename T>
        std::unordered_map<TaskID, std::unique_ptr<Task>> match(T type) {
        
        }
        
        std::unordered_map<TaskID, std::unique_ptr<Task>> m_tasks;
    };
}

