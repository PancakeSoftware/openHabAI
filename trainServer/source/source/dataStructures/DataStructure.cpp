/*
 * File:   DataStructure.h
 * Author: Joshua Johannson
 *
  */

#include <json.hpp>
#include <util/util.h>
#include <dataStructures/DataStructure.h>
#include <Chart.h>
#include <NeuralNetwork.h>
#include <exprtk.hpp>


DataStructure::DataStructure()
: ApiRouteJson(),
  dataChart()
{
  setSubRoutes({{"networks", &networks},
                {"dataChart", &dataChart}});

  setLogName("DATASTRUC");

  // link network when created with this datastructure
  networks.setCreateItemFunction([this](Json params) ->  NeuralNetwork* {
        NeuralNetwork *t = new NeuralNetwork(this);
        t->fromJson(params);
        return t;
  });

  // setup chart
  dataChart.setInputOutputNames({"x", "someInput", "otherInput"}, {"y"});
  dataChart.setUpdateFunctionRanged([this] (const vector<RangeParam> &inputRanges, vector<ValueParam> fixedInputs, const vector<int> &outputIds) {
    map<int, vector<ChartDataPoint>> result;

    // filter records
    // @TODO filter by steps
    for (pair<vector<float>, vector<float>> record : data)
    {
      // if inputs in range
      bool inRange = true;
      for (RangeParam rangeParam : inputRanges) {
        inRange &= (rangeParam.from <= record.first[rangeParam.id]) && (record.first[rangeParam.id] <= rangeParam.to);
        if (!inRange)
          break;
      }
      for (ValueParam fixedParam : fixedInputs) {
        inRange &= (fixedParam.value -fixedParam.tolerance <= record.first[fixedParam.id]) && (record.first[fixedParam.id] <= fixedParam.value + fixedParam.tolerance);
        if (!inRange)
          break;
      }
      if (!inRange)
        continue;

      // add requested outputs
      for (int outputId : outputIds) {
        if (result.find(outputId) == result.end())
          result.emplace(outputId, vector<ChartDataPoint>{});
        result[outputId].push_back(ChartDataPoint(record.second[outputId], record.first));
      }
    }
    return result;
  });
}

DataStructure *DataStructure::create(Json params)
{
  DataStructure *n;
  if (params == NULL)
    return nullptr;

  if (params["type"] == "function")
    n = new FunctionDataStructure();
  else
    return nullptr;

  n->fromJson(params);
  return n;
}


bool DataStructure::operator==(int other) const
{
  return this->id == other;
}
bool DataStructure::operator==(const DataStructure &other) const
{
  return this->id == other.id;
}

/* Json keys */
void DataStructure::params()
{ ApiRouteJson::params();
  param("name", name);
  param("type", type);
  param("id", id);
  // @TODO somehow map<K, V> can only be set if K = string
  // -> use workaround via paramWithFunction
  paramWithFunction("inputNames",
                    [this](Json j) {
                      inputNames.clear();
                      int idNext = 0;
                      for (auto it=j.begin(); it!=j.end(); it++) {
                        if (it.value().is_array())
                        { // contains id
                          inputNames.insert(make_pair(it.value().begin().value(),
                                                      next(it.value().begin()).value().get<string>()));
                          idNext = idNext > it.value().begin().value() ? idNext+1 : ((int)it.value().begin().value())+1;
                        }
                        else
                          inputNames.insert(make_pair(idNext++, it.value().get<string>()));
                      }
                      // update chart
                      dataChart.setInputNames(inputNames);
                    },
                    [this] () { return inputNames;});
  paramWithFunction("outputNames",
                    [this](Json j) {
                      outputNames.clear();
                      int idNext = 0;
                      for (auto it=j.begin(); it!=j.end(); it++) {
                        if (it.value().is_array())
                        { // contains id
                          outputNames.insert(make_pair(it.value().begin().value(),
                                                       next(it.value().begin()).value().get<string>()));
                          idNext = idNext > it.value().begin().value() ? idNext+1 : ((int)it.value().begin().value())+1;
                        }
                        else
                          outputNames.insert(make_pair(idNext++, it.value().get<string>()));
                      }
                      // update chart
                      dataChart.setOutputNames(outputNames);
                    },
                    [this] () { return outputNames;});

  // data-records amount
  paramWithFunction("dataRecords", [](Json j) {}, [this]() { return getDataBatchIndices();});
}





// -- FUCTION -------------------------------

FunctionDataStructure::FunctionDataStructure()
{
}


void FunctionDataStructure::onParamsChanged(vector<string> params)
{
  // re-calc if function or ranges changed
  if (contains(params, {"function", "inputRanges"}))
  {
    debug("("+to_string(id)+") recalc function: " + function);

    // get list with all input combinations
    vector<vector<float>> inputGrid = createInputDataGrid(inputRanges);


    // compile and bind to input vector
    typedef exprtk::symbol_table<float> symbol_table_t;
    typedef exprtk::expression<float>     expression_t;
    typedef exprtk::parser<float>             parser_t;

    vector<float> inputValues(inputNames.size());

    // Register all inputs in the symbol_table
    symbol_table_t symbol_table;
    for (auto in : inputNames) {
      symbol_table.add_variable(in.second, inputValues[in.first]);
      // inputPoint index and inputNames.id have to match!
    }

    // Instantiate expression and register symbol_table
    expression_t expression;
    expression.register_symbol_table(symbol_table);

    // Instantiate parser and compile the expression
    parser_t parser;
    if (!parser.compile(function, expression)) {
      err("compile function '" + function + "': " + parser.error().c_str());
      Catflow::send(ApiRespondError("DataStrucure("+ to_string(id) +"): compile function error '" + function + "': " + parser.error().c_str()));
    }

    // clear old data
    data.clear();

    // for each inputPoint
    for (vector<float> inputPoint : inputGrid)
    {
      // copy values
      for (int i = 0; i < inputPoint.size(); ++i)
        inputValues[i] = inputPoint[i];

      // re-evaluate and add data-record
      data.push_back(make_pair(inputPoint, vector<float>{expression.value()}));
    }
    info("created " + to_string(getDataBatchIndices()) + " data-records");
    info(Json{data}.dump(2));
  }
}