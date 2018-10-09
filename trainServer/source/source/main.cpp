#include <iostream>
#include <condition_variable>
#include <signal.h>
#include <unistd.h>
#include <boost/program_options.hpp>

#include "NeuralNetwork.h"
#include "Catflow.h"
#include <ApiRoot.h>
#include <util/TaskManager.h>
#include <server/ApiSeasocksServer.h>
#include <server/ApiuWebsocketsServer.h>

using namespace std;
using namespace boost::program_options;

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
  /*
   * parse cli
   */
  options_description cliOps{"options"};
  cliOps.add_options()
      ("help,h",                          "show help message")
      ("port,p",          value<int>()->default_value(5555),        "port for api and web gui")
      ("html-files-path", value<string>()->default_value("/usr/share/openhabai/web"),  "path to openhab ai gui html files to serve")
      ("store-path"     , value<string>()->default_value("."),     "path to store all data");
  variables_map cliArgs;
  store(parse_command_line(argc, argv, cliOps), cliArgs);
  notify(cliArgs);

  if (cliArgs.count("help")) {
    std::cout << cliOps << '\n';
    return 0;
  }

  string htmlFilesPath    = cliArgs["html-files-path"].as<string>();
  string configStorePath  = cliArgs["store-path"].as<string>();
  int port                = cliArgs["port"].as<int>();



  /*
   * start app
   */
  mlog.setLogLevel(Log::LOG_LEVEL_ALL);
  mlog.setLogName("main");
  mlog.info("OpenHabAI v0.0");
  mlog.info("for help call 'OpenHabAI -h'");

  // stop main thread at terminate signal
  signal(SIGTERM, atExit);


  // start frontend, load saved
  NeuralNetwork::init(new Context(DeviceType::kCPU, 0));

  ApiRoot apiRoot;
  apiRoot.setStorePath(configStorePath);
  apiRoot.restore();
  Catflow::setApiRootRoute(apiRoot);
  Catflow::start<ApiuWebsocketsServer>(
      port,
      htmlFilesPath // webserver root
  );

  // start blocking taskManager
  TaskManager::start();


  /*
   * shutdown all */
  mlog.info("will shutdown ...");
  NeuralNetwork::shutdown();
  return 0;
}


