/* 
 * File:   TaskManager.h
 * Author: Joshua Johannson
 *
  */
#ifndef OPENHABAI_TASKMANAGER_H
#define OPENHABAI_TASKMANAGER_H

#include <functional>
#include <list>
#include <mutex>
#include <condition_variable>
#include "Log.h"
using namespace std;

class TaskId {
  public:
    TaskId(): isValidV(false) {
    }

    TaskId(list<function<void()>>::iterator iteratorV):
        iterator(iteratorV),
        isValidV(true){
    }

    bool isValid() {
      return isValidV;
    }

    void setValid(bool valid) {
      this->isValidV = valid;
    }

    list<function<void()>>::iterator getIterator() {
      return iterator;
    }
  private:
    bool isValidV;
    list<function<void()>>::iterator iterator;
};

/*
 * TaskManager class
 * add tasks to execute in main thread
 */
class TaskManager
{
  public:


    /**
     * start executing tasks
     * @note this function is blocking
     */
    static void start();

    /**
     * stops blocking of start()
     */
    static void stop();

    /**
     * adds task that is executed repeating, endless
     * @param task task to execute
     * @param id
     */
    static TaskId addTaskRepeating(function<void ()> task);

    /**
     * stops and removes repeating task
     * sets taskId.isValid to false when task has been removed
     * @param taskId
     * @return if task existed
     */
    static bool removeTaskRepeating(TaskId &taskId);

    static bool containsTaskRepeating(TaskId &taskId);

    /**
     * adds task that is executed only once
     * @param task task to execute
     * @param id
     */
    static void addTaskOnceOnly(function<void ()> task, void *id);

    static bool containsTaskOnceOnly(void *id);


  private:
    static Log l;
    static list<TaskId*> tasksRepeatingToRemove;
    static list<function<void ()>> tasksRepeating;
    static list<pair<void *, function<void ()>>> tasksOnceOnly;

    static mutex threadMutex;
    static condition_variable threadLockCondition;

    /*
     * if equals false start() will stop blocking */
    static bool running;
};



#endif //OPENHABAI_TASKMANAGER_H
