#include <iostream>
#include <condition_variable>
#include <signal.h>
#include <unistd.h>

#include "NeuralNetwork.h"
#include "Frontend.h"

using namespace std;

// var
std::mutex mainLockMutex;
condition_variable mainLockCondition;
bool condition = false;

void testData();

void atExit(int code)
{
  cout << "will stop main thread" << endl;
  mainLockCondition.notify_one();
}

/* -- MAIN PROGRAM ------------------------------------------- */
int main(int argc, char *argv[])
{
  // stop main thread at terminate signal
  signal(SIGTERM, atExit);

  // start frontend
  Frontend::start(5555);

  cout << "start" << endl;

  vector<float> x = {0, 1, 2};
  vector<float> y = {5, 2, 3};


  NeuralNetwork *network = nullptr;

  Frontend::onData = [&](Json data)
  {
    if (data["type"] == "do")
    {
      if (data["what"] == "startTrain")
      {
        cout << "=>> start train" << endl;

        if (network != nullptr) {
          Frontend::send("message", {"message", "kill old training"});

          cout << "-- WILL KILL OLD" << endl;

          network->stopTrain();
          network->joinTrainThread();

          delete(network);
          network = nullptr;
        }

        network = new NeuralNetwork(1, 1);

        // feedback
        Frontend::respond("ok", data);

        network->trainInNewThread();
      }

      if (data["what"] == "stopTrain")
      {
        cout << "=>> stop train" << endl;

        if (network != nullptr)
        {
          network->stopTrain();
          network->joinTrainThread();

          delete (network);
          network = nullptr;

          // feedback
          Frontend::respond("ok", data);
        }
      }
    }
  };


  //testData();

  // wait until terminate signal
  unique_lock<std::mutex> lock(mainLockMutex);
  mainLockCondition.wait(lock);

  NeuralNetwork::shutdown();

  return 0;
}



void testData()
{
  // test data
  for (float i = 10; i < 100; i+=0.01)
  {
    Json data = {
        {"chart", "progress"},
        {"data", {}}
    };

    data["data"].push_back(
        {
            {"graph", "error"},
            { "x", i },
            { "y", sin(i*i) }
        }
    );

    data["data"].push_back(
        {
            {"graph", "test"},
            { "x", i },
            { "y", i*cos(i*i) }
        }
    );

    Frontend::sendData(data);

    usleep(400000);
  }
}