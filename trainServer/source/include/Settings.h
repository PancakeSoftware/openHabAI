//
// Created by Tristan Ratz on 29.05.18.
//

#ifndef OPENHABAI_SETTINGS_H
#define OPENHABAI_SETTINGS_H

#include <string>
#include <ApiJsonObject.h>
#include <util/TaskManager.h>
#include "SelectorView.h"

using namespace std;

class Settings : public ApiJsonObject
{
  public:
    int trainRefreshRate{1000};   // The rate of how often the terminal will output the learn Progress
    SelectorView logLevel;        // The level of log
    SelectorView computingTarget; // Where the computation should run (CPU/GPU)
    SelectorView showConsole;     // show network console in frontend

    Settings() :
        computingTarget({ "CPU", "GPU" }),
        logLevel({"all", "trace", "info", "debug", "ok", "warning", "error", "fatal"}),
        showConsole({"show", "hide"})

    {
      setLogName("Settings");

      addAction("stopServer", [this](ApiRequest request) {
        info("Stopping server...");
        TaskManager::stop(); // stop app
        return new ApiRespondOk(request);
      });

      logLevel.onSelectionChange([&](vector<bool> values, vector<bool> changedValues) {
        setLogFilter(values);
      });

    }

  private:
    void params() override
    {
      JsonObject::params();
      param("trainRefreshRate", trainRefreshRate);
      param("computingTarget", computingTarget);
      param("logLevel", logLevel);
      param("showConsole", showConsole);
    }

    void setLogFilter(vector<bool> values) {
      int index = 1; // ALL (Standard)
      for (int i = 0; i < values.size(); i++)
      {
        if (values[i])
        {
          index = i;
          break;
        }
      }
      info("set log level to: " + to_string(index));

      switch (index) {
        case 0:
          Log::setLogLevel(Log::LOG_LEVEL_ALL);
          break;
        case 1:
          Log::setLogLevel(Log::LOG_LEVEL_TRACE);
          break;
        case 2:
          Log::setLogLevel(Log::LOG_LEVEL_INFORMATION);
          break;
        case 3:
          Log::setLogLevel(Log::LOG_LEVEL_DEBUG);
          break;
        case 4:
          Log::setLogLevel(Log::LOG_LEVEL_OK);
          break;
        case 5:
          Log::setLogLevel(Log::LOG_LEVEL_WARNING);
          break;
        case 6:
          Log::setLogLevel(Log::LOG_LEVEL_ERROR);
          break;
        case 7:
          Log::setLogLevel(Log::LOG_LEVEL_FATAL);
          break;
        default:
          Log::setLogLevel(Log::LOG_LEVEL_ALL);
          break;
      }
    }

};

#endif //OPENHABAI_SETTINGS_H
