/* 
 * File:   Chart.h
 * Author: Joshua Johannson
 *
 */
#ifndef OPENHABAI_CHART_H
#define OPENHABAI_CHART_H

#include "ApiJsonObject.h"
#include "Frontend.h"
class RangeParam;
class ValueParam;
class ChartDataPoint;


/*
 * Chart class
 */
class Chart: public ApiJsonObject
{
  public:
    Chart();

    /*
     * Json keys */
    void params() override { ApiJsonObject::params();
    }

    ApiRespond *processApi(ApiRequest request) override;

    /**
     * send change data to subscribers
     */
    void pushUpdate();
};

/*
 * chart for multidimensional data
 * all is updated
 */
class ParameterChart: public Chart
{
  public:
    ParameterChart();

    /*
     * Json keys */
    void params() override { Chart::params();
      paramReadOnly("inputNames", inputNames);
      paramReadOnly("outputNames", outputNames);
      param("fixedInputs", fixedInputs);
      param("rangeInputs", rangeInputs);
      param("outputs", outputs);
    }

    /**
     * set input/output names and their Id
     * @param inputNames
     * @param outputNames
     */
    void setInputOutputNames(map<int, string> inputNames, map<int, string> outputNames) {
      this->inputNames = inputNames;
      this->outputNames = outputNames;
    }

    /**
     * set input/output names, id is set automatic
     * @param inputNames
     * @param outputNames
     */
    void setInputOutputNames(vector<string> inputNames, vector<string> outputNames) {
      this->inputNames.clear();
      this->outputNames.clear();
      int i = 0;
      for (string name : inputNames)
        this->inputNames.emplace(i++, name);
      i = 0;
      for (string name : outputNames)
        this->outputNames.emplace(i++, name);
    }

    /**
     * set the function for generating chart data
     * @param func generates for given inputValue( map<inputID, value> ) outputValue( map<outputID, value> )
     */
    void setUpdateFunction(function<map<int, float> (const map<int, float> &inputValues, const vector<int> &outputIds)> func) {
      this->updateFunc = func;
    }

    ApiRespond *processApi(ApiRequest request) override;

  private:
    map<int, string> inputNames;
    map<int, string> outputNames;

    vector<ValueParam> fixedInputs;
    vector<RangeParam> rangeInputs;
    vector<int> outputs;
    map<int, vector<ChartDataPoint>> data;

    function<map<int, float> (const map<int, float> &inputValue, const vector<int> &outputIds)> updateFunc;
};


/*
 * chart for 2D data
 * x-axis = time
 * add dataPoints over time
 */
class TimeChart: public Chart
{

};


class RangeParam: public JsonObject{
  public:
    float from = 0;
    float to = 0;
    int steps = 1;
    int id;

    void params() override {
      param("id", id);
      param("from", from);
      param("to", to);
      param("steps", steps);
    }
};

class ValueParam: public JsonObject{
  public:
    int id;
    float value;

    void params() override {
      param("id", id);
      param("value", value);
    }
};

class ChartDataPoint: public JsonObject{
  public:
    float value;
    map<int, float> inputs;

    ChartDataPoint(float value, map<int, float> inputs) : value(value), inputs(inputs) {}

    void params() override {
      param("value", value);
      paramReadOnly("inputs", inputs);
    }
};


#endif //OPENHABAI_CHART_H
