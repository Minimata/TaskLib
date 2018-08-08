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
        
        /**
         * TODO
         *
         * General:
         * Clean the repetitivity of the methods in this class, and stop returning ifSuccess, instead throw an exception
         * Clean up the example main and make the tester available to the console
         * Test what can be tested
         *
         * Methods:
         * Add join and joinAll methods
         * Add setFunction and setCallback methods
         * Add the possibility to set Function and Callback with the start method
         * Add removeTask and removeAllTasks methods
         *
         * Features:
         * Make Tasks are chainable, for example:
         * createTask({doThis();}, {doThatNext();}, {doThisFinally();});
         * Make the start method return a promise
         *
         */
    
        /// Overly repetitive
        bool start(TaskID id) {
            if(m_tasks.find(id) != m_tasks.end()) {
                m_tasks.at(id)->start();
                return true;
            }
            else {
                std::cout << "No task with ID " << id << std::endl;
                return false;
            }
        }
        bool pause(TaskID id) {
            if(m_tasks.find(id) != m_tasks.end()) {
                m_tasks.at(id)->pause();
                return true;
            }
            else {
                std::cout << "No task with ID " << id << std::endl;
                return false;
            }
        }
        bool resume(TaskID id) {
            if(m_tasks.find(id) != m_tasks.end()) {
                m_tasks.at(id)->resume();
                return true;
            }
            else {
                std::cout << "No task with ID " << id << std::endl;
                return false;
            }
        }
        bool stop(TaskID id) {
            if(m_tasks.find(id) != m_tasks.end()) {
                m_tasks.at(id)->stop();
                return true;
            }
            else {
                std::cout << "No task with ID " << id << std::endl;
                return false;
            }
        }
        bool status(TaskID id) {
            if(m_tasks.find(id) != m_tasks.end()) {
                std::cout << m_tasks.at(id)->getState() << std::endl;
                return true;
            }
            else {
                std::cout << "No task with ID " << id << std::endl;
                return false;
            }
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
        void allStatuses() {
            for (const auto& task : m_tasks) {
                std::cout << task.second->getID() << " : " << task.second->getState() << std::endl;
            }
        }
    
    private:
        std::unordered_map<TaskID, std::unique_ptr<Task>> m_tasks;
    };
}

