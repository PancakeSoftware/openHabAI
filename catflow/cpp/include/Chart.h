/* 
 * File:   Chart.h
 * Author: Joshua Johannson
 *
 */
#ifndef OPENHABAI_CHART_H
#define OPENHABAI_CHART_H

#include "ApiJsonObject.h"
#include "Catflow.h"
class RangeParam;
class ValueParam;
class ChartDataPoint;
class ChartDataPointSimple;


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
      paramReadOnly("inputNames", inputNames);
      paramReadOnly("outputNames", outputNames);
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
    void setInputNames(map<int, string> inputNames) {
      this->inputNames = inputNames;
    }
    void setOutputNames(map<int, string> outputNames) {
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

    ApiRespond *processApi(ApiRequest request) override;

    /**
     * send change data to subscribers
     */
    void pushUpdate();

  protected:
    map<int, string> inputNames;
    map<int, string> outputNames;
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
      param("fixedInputs", fixedInputs);
      param("rangeInputs", rangeInputs);
      param("outputs", outputs);
    }

    /**
     * set the function for generating chart data
     * @param func generates for given inputValue( map<inputID, value> ) outputValue( map<outputID, value> )
     */
    void setUpdateFunction(function<map<int, float> (const map<int, float> &inputValues, const vector<int> &outputIds)> func) {
      this->updateFunc = func;
    }

    /**
     * set the function for generating chart data
     * @param func returns outputValues( map<outputID, value> ) by given ranges
     * @note input and outputId represented by index
     */
    void setUpdateFunctionRanged(function<map<int, vector<ChartDataPoint>> (const vector<RangeParam> &inputRanges, vector<ValueParam> &fixedInputs, const vector<int> &outputIds)> func) {
      this->updateFuncRanged = func;
    }

    /**
     * execute update function
     * @note blocking
     */
    void recalcData();

    ApiRespond *processApi(ApiRequest request) override;

  private:
    vector<ValueParam> fixedInputs;
    vector<RangeParam> rangeInputs;
    vector<int> outputs;
    map<int, vector<ChartDataPoint>> data;

    function<map<int, float> (const map<int, float>    &inputValue, const vector<int> &outputIds)> updateFunc{nullptr};
    function<map<int, vector<ChartDataPoint>> (const vector<RangeParam> &inputRanges, vector<ValueParam> &fixedInputs, const vector<int> &outputIds)> updateFuncRanged{nullptr};
};


/*
 * chart for 2D data
 * x-axis =
 * add dataPoints over time
 */
class SeriesChart: public Chart
{
  public:
    SeriesChart();

    /**
     * add dataPoint to chart
     * @param outputId
     * @param inputValues
     * @param outputValue
     */
    void addDataPoint(int outputId, vector<float> inputValues, float outputValue);

    /**
     * add dataPoint to chart
     * @param outputName
     * @param inputValues
     * @param outputValue
     */
    void addDataPoint(string outputName, vector<float> inputValues, float outputValue);;

    /**
     * delete all data form chart
     */
    void clearData();

  protected:
    map<int, vector<ChartDataPointSimple>> data;


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
    float tolerance;

    void params() override {
      param("id", id);
      param("value", value);
      param("tolerance", tolerance);
    }
};

class ChartDataPoint: public JsonObject{
  public:
    float value;
    map<int, float> inputs;

    ChartDataPoint(float value, map<int, float> inputs) : value(value), inputs(inputs) {}
    ChartDataPoint(float value, vector<float> inputsVec) : value(value), inputs() {
      int i=0;
      for (auto input = inputsVec.begin(); input != inputsVec.end(); input++) {
        inputs.emplace(i, *input);
        i++;
      }
    }

    void params() override {
      param("value", value);
      paramReadOnly("inputs", inputs);
    }
};

class ChartDataPointSimple: public JsonObject{
  public:
    float value;
    vector<float> inputs;

    ChartDataPointSimple(float value, vector<float> inputs) : value(value), inputs(inputs) {}

    void params() override {
      param("value", value);
      paramReadOnly("inputs", inputs);
    }
};

#endif //OPENHABAI_CHART_H
