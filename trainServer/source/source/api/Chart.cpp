/* 
 * File:   Chart.h
 * Author: Joshua Johannson
 *
 */
#include <api/ApiMessage.h>
#include <util/TaskManager.h>
#include <Frontend.h>
#include "api/Chart.h"


Chart::Chart()
{
}

ApiRespond *Chart::processApi(ApiRequest request)
{
  ApiRespond *respond = ApiJsonObject::processApi(request);
  return respond;
}

void Chart::pushUpdate()
{

}


/**
 * -- ParameterChart -------------------------------------
 */
ParameterChart::ParameterChart()
{
}
ApiRespond *ParameterChart::processApi(ApiRequest request)
{
  ApiRespond *respond = Chart::processApi(request);

  // when chart params change
  if (request.what == "update")
  {
    if (updateFunc == nullptr)
      return new ApiRespondError("can't update chart data: updateFunction(used to generate output data from rangeInputs) is not set", request);

    // update chart data
    // @TODO if calc update is already in tasklist replace it
    TaskManager::addTaskOnceOnly([this, route = this->route, &data = this->data, outputs = this->outputs, fixedInputs = this->fixedInputs, rangeInputs = this->rangeInputs, &updateFunc = this->updateFunc]() {
      map<int, vector<ChartDataPoint>> dataOut;

      // for each rageInput
      function<void(vector<RangeParam>::const_iterator, map<int, float>)> createData =
          [&](vector<RangeParam>::const_iterator input, map<int, float> inputData)
          {
            if (input == rangeInputs.end())
            {
              //debug("create dataPoint at "+ Json{inputData}.dump());
              for (auto i: updateFunc(inputData, outputs))
                dataOut[i.first].push_back(ChartDataPoint(i.second, inputData));
              return;
            }

            // calc increase step for rangeInput
            float step;
            if (input->steps > 0)
              step = ((input->to - input->from) / input->steps);
            else
              step = 0;
            step = (step > 0) ? step : 0;
            //debug("progress input (perStep+="+to_string(step)+")"+ input->toJson().dump());

            // for each point of range
            for (float i = input->from; i <= input->to; i += step)
            {
              map<int, float> nInputData = inputData;
              nInputData.emplace(input->id, i);
              createData(input+1, nInputData);
              if (step == 0)
                break;
            }
          };

      // remove old data
      // data.clear();

      // generate new data
      map<int, float> inputDat;
      for (auto i: fixedInputs) // insert fixedInputs
        inputDat.emplace(i.id, i.value);
      createData(rangeInputs.begin(), inputDat); // generate data by rangeInputs
      //debug("will send to Frontend: " + route.get().toString() + "  =   " + to_string(data.size()) + Json(data).dump(2));

      // send data, update data in object
      sendToSubscribers(ApiRequest(route.get(), "updateData", dataOut));
      data = dataOut;
    });
  }

  return respond;
}
