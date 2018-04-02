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
: ApiRouteJson({{"networks", &networks},
                /*{"dataChart", &dataChart}*/}),
  dataChart()
{
  setLogName("DATASTRUC");
  // link network when created with this datastructure
  networks.setCreateItemFunction([this](Json params) ->  NeuralNetwork* {
        NeuralNetwork *t = new NeuralNetwork(this);
        t->fromJson(params);
        return t;
  });

  // setup chart
  dataChart.setUpdateFunction([this] (map<int, float> &inputValues, vector<int> &outputIds) {
    // @TODO ineffective
    vector<float> in;
    for (auto i: inputValues)
      in[i.first] = i.second;
    vector<float> out = getDataBatch(in);
    map<int, float> outPut;
    for (auto id: outputIds) {
      if (out.size() < id)
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



// -- FUCTION -------------------------------

FunctionDataStructure::FunctionDataStructure()
{
}

vector<float> FunctionDataStructure::getDataBatch(vector<float> input)
{
  cout << "get Batch: " << this->function << endl;

  typedef exprtk::symbol_table<double> symbol_table_t;
  typedef exprtk::expression<double>     expression_t;
  typedef exprtk::parser<double>             parser_t;


  vector<float> result;

  double x;

  // Register x with the symbol_table
  symbol_table_t symbol_table;
  symbol_table.add_variable("x",x);

  // Instantiate expression and register symbol_table
  expression_t expression;
  expression.register_symbol_table(symbol_table);

  // Instantiate parser and compile the expression
  parser_t parser;
  parser.compile(function, expression);


  // evaluate
  for (float val : input)
  {
    x = val;
    result.push_back(expression.value());
  }

  return result;
}

