/* 
 * File:   TaskManager.h
 * Author: Joshua Johannson
 *
 */
#include <iostream>
#include "util/TaskManager.h"
bool TaskManager::running = true;
Log TaskManager::l("TaskManager");
list<TaskId*> TaskManager::tasksRepeatingToRemove;
list<function<void ()>> TaskManager::tasksRepeating;
list<pair<void *, function<void ()>>> TaskManager::tasksOnceOnly;
mutex TaskManager::threadMutex;
condition_variable TaskManager::threadLockCondition;

/*
 * if equals false start() will stop blocking */
static bool running;


TaskId TaskManager::addTaskRepeating(function<void()> task)
{
  tasksRepeating.push_back(task);
  list<function<void ()>>::iterator last = --tasksRepeating.end();
  threadLockCondition.notify_one();
  return TaskId(last);
}

void TaskManager::addTaskOnceOnly(function<void()> task, void *id)
{
  tasksOnceOnly.push_back(make_pair(id, task));
  //l.debug("tasksOnceOnly: " + to_string(tasksOnceOnly.size()));
  threadLockCondition.notify_one();
}

void TaskManager::start()
{
  running = true;

  while (running)
  {
    // execute repeating tasks
    for (auto task : tasksRepeating)
      task();

    // remove repeating tasks, that are on removeList
    for (auto task = tasksRepeatingToRemove.begin(); task != tasksRepeatingToRemove.end();) {
      tasksRepeating.erase((*task)->getIterator());
      (*task)->setValid(false);
      task = tasksRepeatingToRemove.erase(task);
    }

    // execute once only tasks
    for (auto it = tasksOnceOnly.begin(); it != tasksOnceOnly.end(); ) {
      (*it).second();
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

bool TaskManager::containsTaskOnceOnly(void *id)
{
  for (pair<void *, function<void()>> el : tasksOnceOnly) {
    if (el.first == id)
      return true;
  }
  return false;
}

bool TaskManager::removeTaskRepeating(TaskId &taskId)
{
  for (auto i = tasksRepeating.begin(); i != tasksRepeating.end() ; ++i) {
    if (i == taskId.getIterator()) {
      tasksRepeatingToRemove.push_back(&taskId);
      threadLockCondition.notify_one(); // wakeup
      return true;
    }
  }
  return false;
}

bool TaskManager::containsTaskRepeating(TaskId &taskId)
{
  for (auto i = tasksRepeating.begin(); i != tasksRepeating.end() ; ++i) {
    if (i == taskId.getIterator()) {
      return true;
    }
  }
  return false;
}
