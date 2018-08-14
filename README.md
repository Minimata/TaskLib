# TaskLib

A small C++ Library to manage asynchronous tasks.

## Features

This C++ header-only library has been developed to offer more control on asynchronous tasks being launched in a program.

With TaskLib you can:

- use a function pointer or wrap any function in a lambda and launch it as a thread concurrently by creating a task
- choose when to start, pause, resume and stop a task
- check in what state a task is (paused, running, stopped or completed)
- assign a callback to the task that that will run concurrently too
- assign any type to the tasks you create to classify them
- make use of the task manager to manipulate individual tasks by ID, groups of tasks by type, or all tasks at once.

## Usage

The usual way to use the library is through the `TaskManager` class, which is essentially a collection of `Tasks`.
You can however use the `Task` class directly, if you prefer.

Here are a few examples of use.

### Simple application

```
#include <iostream>
#include "TaskManager.h"

int main() {
    TaskLib::TaskManager taskManager;

    auto id = taskManager.createTask([](){ std::cout << "Hello asynchronous world !" << std::endl; });
    taskManager.start(id);
    taskManager.join(id);

    return 0;
}
```

Outputs

```
Hello asynchronous world !
```

### More complex example with imaginary data

```
#include <iostream>
#include <vector>

#include "DataProcessor.h"
#include "DataContainer.h"
#include "CustomSensors.h"
#include "CustomLogger.h"
#include "TaskManager.h"

int main() {
    TaskLib::TaskManager taskManager;
    std::vector<TaskLib::TaskID> ids;
    DataProcessor dataProcessor;
    CustomLogger logger;

    // Create a processor task for each set of data
    for(const auto& data : DataContainer::getData()) {
        auto id = taskManager.createTask([&](){ dataProcessor.process(data); });
        taskManager.setType(id, std::string("processor"));
        ids.push_back(id);
    }
    // Create a task for each custom sensor
    for(const auto& sensor : CustomSensors::getSensors()) {
        auto id = taskManager.createTask([&](){ sensor.start(); });
        ids.push_back(id);
    }
    // Sets a logger callback on each created task
    for(const auto& id : ids) taskManager.setCallback(id, [&](){ logger.log(id); });

    // Start the processor tasks and wait for them to finish
    taskManager.startTaskOfType(std::string("processor"));
    taskManager.joinAllTasks();

    // Start the tasks that haven't been completed yet, meaning the sensor ones
    taskManager.startAllTasks();

    // Wait on the tasks to finish and clean up the task manager for future use
    taskManager.joinAllTasks();
    taskManager.removeAllTasks();

    /*...*/

    return 0;
}
```

## Notes

### On Task states

Tasks begin in state `paused`.
They can go from `paused` to `running` and vice versa.
A `running` task can get `completed`, and a `running` or `paused` task can be `stopped`.
Once `stopped` or `completed`, a task cannot change state.

### On Task Types

As of today 14th of August 2018 and for simplicity reasons, a task's type can't be a pointer.
If you choose a custom object as a task's type, make sure:
- it's not too heavy to copy
- it implements `operator<<` properly
- you don't expect a polymorphic use out of it.

Also, note that if your custom object doesn't implement `operator==`,
you can still pass a lambda to any method `[action]TaskOfType` of the `TaskManager` (e.g. `startTaskOfType`) that will describe the comparison.
In that case, the lambda you pass must take two arguments of the same type by value and return the result of the comparison, as shown below with the method `compareType` of class `Task`.

#### Example

```
#include <iostream>
#include "Task.h"

class Tester {
public:
    virtual int compare() const { return 42; }
    friend std::ostream& operator<<(std::ostream& os, const TaskTester& t) {
        os << "TaskTester";
        return os;
    }
};

class ChildTester : public Tester {
public:
    int compare() const override { return 64; }
    friend std::ostream& operator<<(std::ostream& os, const TaskChildTester& t) {
        os << "TaskChildTester";
        return os;
    }
};

int main() {
    TaskLib::Task task(0);

    task.setType(ChildTester());

    // Returns true
    task.compareType(ChildTester(), [](ChildTester t1, ChildTester t2){ return t1.compare() == t2.compare(); });
    // Returns true as well, even though it shouldn't be the case if polymorphic use was supported
    task.compareType(Tester(), [](Tester t1, Tester t2){ return t1.compare() == t2.compare(); });

    return 0;
}
```

### On `pause` and `resume`

As far as I know, it is not possible to pause (and therefore resume) a thread from another thread in the C++ standard, and for good reasons.
If you want your threads to be paused and wait on something, you can either use [mutexes](http://www.cplusplus.com/reference/mutex/mutex/) and [condition variables](https://fr.cppreference.com/w/cpp/thread/condition_variable) or [promises with futures](https://en.cppreference.com/w/cpp/thread/promise).

However, all of these methods require some insight in the function that is being executed asynchronously.
Since this library accepts any kind of function as a template, it has no way to properly make the thread wait on something by the C++ standard.
It is the responsability of the user to do so, which is nice because it keeps the code efficient.

Therefore, the `pause` and `resume` methods are not what they pretend to be.
The pause method actually stops the thread, but instead of moving the `Task`'s state to `stopped` it changes back to `paused`.
The `resume` method however is exactly the same as the `start` method.
Both methods are only here to respect the state machine, but they do not pause and resume, they stop and restart the task.

## How to include TaskLib in my project

The simplest method is to copy the content of `src/TaskHandling` directly in the source of your project.
Note that the library is header only, as it relies quite heavily on templates.

### Possible changes you can do

#### C++ 14 users

If you compile using the C++14 (or above) standard, which is great, you can get rid of the `src/TaskHandling/CPP11Helpers.h` file.
This file is only there to use the `make_unique` function when compiling with C++11 flags.
If you choose not to use that file, make sure to replace every occurrence of `CPP11Helpers::` by `std::` in the `src/TaskHandling/TaskManager.h` and `src/TaskHandling/Task.h` files.

#### MinGW users

If you compile on Windows with MinGW, make sure you also copy the files in the `thread` folder.
MinGW doesn't implement the C++ standard for thread by default, but these two small files make up for it.
The source for this MinGW thread library are [here](https://github.com/Minimata/TaskLib).
