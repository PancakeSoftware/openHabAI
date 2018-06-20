//
// Created by Tristan Ratz on 29.05.18.
//

#ifndef OPENHABAI_SETTINGS_H
#define OPENHABAI_SETTINGS_H

#include <string>
#include <ApiJsonObject.h>
#include "SelectorView.h"

using namespace std;

class Settings : public ApiJsonObject
{
  public:
    int trainRefreshRate{1000};         // The rate of how often the terminal will output the learn Progress
    SelectorView logLevel;        // The level of log
    SelectorView computingTarget; // Where the computation should run (CPU/GPU)

    Settings() :
        computingTarget("Computing Target", { "CPU", "GPU" }),
        logLevel("Log Level", {"none", "all", "debug", "error", "fatal", "info", "ok", "trace", "warning"})
    {
      addAction("stopServer", [](ApiRequest request) {
        exit(0);
        return new ApiRespondOk(request);
      });

      logLevel.onSelectionChange([&](vector<bool> values, vector<bool> changedValues) {
        cout<<"Changed Value";
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

      setLogFilter(logLevel.getCurrentValues());
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
      cout<<"Index: "<< index;
      switch (index) {
        case 0:
          Log::setLogLevel(Log::LOG_LEVEL_FATAL);
          break;
        case 1:
          Log::setLogLevel(Log::LOG_LEVEL_ALL);
          break;
        case 2:
          Log::setLogLevel(Log::LOG_LEVEL_DEBUG);
          break;
        case 3:
          Log::setLogLevel(Log::LOG_LEVEL_ERROR);
          break;
        case 4:
          Log::setLogLevel(Log::LOG_LEVEL_FATAL);
          break;
        case 5:
          Log::setLogLevel(Log::LOG_LEVEL_INFORMATION);
          break;
        case 6:
          Log::setLogLevel(Log::LOG_LEVEL_OK);
          break;
        case 7:
          Log::setLogLevel(Log::LOG_LEVEL_TRACE);
          break;
        case 8:
          Log::setLogLevel(Log::LOG_LEVEL_WARNING);
          break;
        default:
          Log::setLogLevel(Log::LOG_LEVEL_ALL);
          break;
      }
    }

};

#endif //OPENHABAI_SETTINGS_H
