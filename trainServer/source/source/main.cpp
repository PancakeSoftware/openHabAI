#include <iostream>
#include <condition_variable>
#include <signal.h>
#include <unistd.h>

#include "NeuralNetwork.h"
#include "Catflow.h"
#include <ApiRoot.h>
#include <util/TaskManager.h>
#include <server/ApiSeasocksServer.h>
#include <server/ApiuWebsocketsServer.h>

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

  apiRoot.setStorePath("./");
  apiRoot.restore();
  Catflow::setApiRootRoute(apiRoot);
  Catflow::start<ApiuWebsocketsServer>(
      5555 // webSocket port
  );

  // start blocking taskManager
  TaskManager::start();


  /*
   * shutdown all */
  mlog.info("will shutdown ...");
  NeuralNetwork::shutdown();
  return 0;
}


