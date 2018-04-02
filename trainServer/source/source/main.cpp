#include <iostream>
#include <condition_variable>
#include <signal.h>
#include <unistd.h>

#include "NeuralNetwork.h"
#include "Frontend.h"
#include <ApiRoot.h>
#include <util/TaskManager.h>

using namespace std;

// var
std::mutex mainLockMutex;
condition_variable mainLockCondition;
bool condition = false;
Log mlog;

void testData();

void atExit(int code)
{
  mlog.info("ON_EXIT: will stop main thread ...");
  TaskManager::stop();
}

/* -- MAIN PROGRAM ------------------------------------------- */
int main(int argc, char *argv[])
{
  mlog.setLogLevel(Log::LOG_LEVEL_ALL);
  mlog.setLogName("main");
  mlog.info("OpenHabAI v0.0");

  // stop main thread at terminate signal
  signal(SIGTERM, atExit);


  // start frontend, load saved
  NeuralNetwork::init(new Context(DeviceType::kCPU, 0));
  Frontend::start(
      5555, // webSocket port
      8050  // http port
  );
  apiRoot.setStorePath("./");
  apiRoot.restore();


  // start blocking taskManager
  TaskManager::start();


  /*
   * shutdown all */
  mlog.info("will shutdown ...");
  NeuralNetwork::shutdown();
  return 0;
}


