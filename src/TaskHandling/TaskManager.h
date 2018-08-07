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
    
        /// Overly repetitive
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
        
        State statusOfTask(TaskID id) {
            return m_tasks.at(id)->getState();
        }
    
    private:
        std::unordered_map<TaskID, std::unique_ptr<Task>> m_tasks;
    };
}

