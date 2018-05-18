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
using namespace std;


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
     */
    static void addTaskRepeating(function<void ()> task);

    /**
     * adds task that is executed only once
     * @param task task to execute
     */
    static void addTaskOnceOnly(function<void ()> task);


  private:
    static list<function<void ()>> tasksRepeating;
    static list<function<void ()>> tasksOnceOnly;

    static mutex threadMutex;
    static condition_variable threadLockCondition;

    /*
     * if equals false start() will stop blocking */
    static bool running;
};


#endif //OPENHABAI_TASKMANAGER_H
