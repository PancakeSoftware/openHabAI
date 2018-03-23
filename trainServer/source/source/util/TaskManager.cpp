/* 
 * File:   TaskManager.h
 * Author: Joshua Johannson
 *
 */
#include <iostream>
#include "util/TaskManager.h"
bool TaskManager::running = true;
list<function<void ()>> TaskManager::tasksRepeating;
list<function<void ()>> TaskManager::tasksOnceOnly;
mutex TaskManager::threadMutex;
condition_variable TaskManager::threadLockCondition;

/*
 * if equals false start() will stop blocking */
static bool running;


void TaskManager::addTaskRepeating(function<void()> task)
{
  tasksRepeating.push_back(task);
  threadLockCondition.notify_one();
}

void TaskManager::addTaskOnceOnly(function<void()> task)
{
  tasksOnceOnly.push_back(task);
  threadLockCondition.notify_one();
}

void TaskManager::start()
{
  running = true;

  while (running)
  {
    for (auto task : tasksRepeating)
      task();

    for (auto it = tasksOnceOnly.begin(); it != tasksOnceOnly.end(); ) {
      (*it)();
      it = tasksOnceOnly.erase(it);
    }

    /*
     * lock when noting to do */
    if (tasksOnceOnly.size() <= 0 && tasksRepeating.size() <= 0) {
      unique_lock<mutex> mlock(threadMutex);
      threadLockCondition.wait(mlock);  // lock mutex
    }
  }
}

void TaskManager::stop()
{
  running = false;
  threadLockCondition.notify_one(); // wakeup
}
