/*
 * File:   DataStructure.h
 * Author: Joshua Johannson
 *
  */

#include <json.hpp>
#include <dataStructures/DataStructure.h>
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
  dataChart.setUpdateFunction([this] (const map<int, float> &inputValues, const vector<int> &outputIds) {
    // @TODO ineffective
    auto maxInputId = std::max_element(inputValues.begin(), inputValues.end(),
                              [](const pair<int, float>& p1, const pair<int, float>& p2) {
                                return p1.first < p2.first; });
    maxInputId->first;

    // input Vector index=id
    //debug("inputs: " + Json(inputValues).dump(2));
    vector<float> in;
    for (int j = 0; j <= maxInputId->first; ++j) {
      //debug("insert ... " + to_string(j));
      in.push_back((inputValues.find(j) != inputValues.end()) ? inputValues.find(j)->second : 0);
    }
    /*)
    for (auto i: inputValues) {
      in[i.first] = i.second;
      //in.insert(in.begin() + i.first, i.second);
    }*/

    // get data point
    vector<float> out = getDataBatch(in);

    // convert back to id map
    map<int, float> outPut;
    for (auto id: outputIds) {
      if (out.size() >= id)
      outPut.emplace(id, out[id]);
    }
    return outPut;
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
}





// -- FUCTION -------------------------------

FunctionDataStructure::FunctionDataStructure()
{
}

vector<float> FunctionDataStructure::getDataBatch(vector<float> input)
{
  //cout << "get Batch: " << this->function << endl;

  typedef exprtk::symbol_table<double> symbol_table_t;
  typedef exprtk::expression<double>     expression_t;
  typedef exprtk::parser<double>             parser_t;


  vector<float> result;

  vector<double > inputs;
  // copy input values
  for (float val : input)
    inputs.push_back(val);

  // Register all inputs in the symbol_table
  symbol_table_t symbol_table;
  for (auto in : inputNames) {
    symbol_table.add_variable(in.second, inputs[in.first]);
  }

  // Instantiate expression and register symbol_table
  expression_t expression;
  expression.register_symbol_table(symbol_table);

  // Instantiate parser and compile the expression
  parser_t parser;
  parser.compile(function, expression);


  // evaluate
  result.push_back(expression.value());

  return result;
}

