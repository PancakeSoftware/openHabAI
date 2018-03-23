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

void testData();

void atExit(int code)
{
  cout << "will stop main thread" << endl;
  TaskManager::stop();
}

/* -- MAIN PROGRAM ------------------------------------------- */
int main(int argc, char *argv[])
{
  // stop main thread at terminate signal
  signal(SIGTERM, atExit);


  // start frontend, load saved
  NeuralNetwork::init(new Context(DeviceType::kCPU, 0));
  Frontend::start(5555);
  apiRoot.setStorePath({});
  apiRoot.restore();


  // start blocking taskManager
  TaskManager::start();


  /*
   * shutdown all */
  NeuralNetwork::shutdown();
  return 0;
}


