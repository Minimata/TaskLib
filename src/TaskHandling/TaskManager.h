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
         * User defined Taskclasses
         * Make the start method return a promise
         *
         */
         
        
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
    
        void start(const TaskID& id) {
            execute([&id, this](){startTask(m_tasks.at(id));}, id);
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
            for (const auto& task : m_tasks)
                std::cout << task.second->getID() << " : " << task.second->getState() << std::endl;
        }
    
    private:
        std::string stateToString(const State& s) { return stateNames[s]; }
        
        std::unordered_map<TaskID, std::unique_ptr<Task>> m_tasks;
    };
}

