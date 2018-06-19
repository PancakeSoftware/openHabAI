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
      //addAction("getCPURate", [] (ApiRequest request) {
      //    return new ApiRespondOk(Json {{ "rate", 0 }}, request);
      //});
    }

  private:
    void params() override
    {
      JsonObject::params();
      param("trainRefreshRate", trainRefreshRate);
      param("computingTarget", computingTarget);
      param("logLevel", logLevel);
    }

};

#endif //OPENHABAI_SETTINGS_H
