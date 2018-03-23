/* 
 * File:   Chart.h
 * Author: Joshua Johannson
 *
 */
#ifndef OPENHABAI_CHART_H
#define OPENHABAI_CHART_H

#include "JsonObject.h"
struct RangeParam;


/*
 * Chart class
 */
class Chart: public JsonObject
{
  public:
    Chart();
    ApiRespond *processApi(ApiRequest request) override;

  private:

};

class ParameterChart: public Chart
{
  public:
    ParameterChart();

    ApiRespond *processApi(ApiRequest request) override;

  private:
    vector<int> fixedInputs;
    vector<RangeParam> rangeInputs;
    vector<int> outputs;
    float range_from;
};

struct RangeParam {
    float from;
    float to;
    int id;
};


#endif //OPENHABAI_CHART_H
