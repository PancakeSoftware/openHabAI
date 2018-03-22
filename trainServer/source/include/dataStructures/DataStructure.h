/* 
 * File:   DataStructure.h
 * Author: Joshua Johannson
 *
  */
#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include <json.hpp>
#include <string>
#include <json/JsonObject.h>
#include <json/JsonList.h>
#include <json/ApiRoute.h>
#include "NeuralNetwork.h"
using namespace std;


/*
 * DataStructure class
 * provides/generates training data
 */
class DataStructure : public ApiRouteJson
{
  public:
    string name;
    string type;
    int id;

    JsonList<NeuralNetwork> networks;

    DataStructure();
    static DataStructure * create(Json params); // create right structure type by param 'type'

    inline bool operator == (int other) const;
    inline bool operator == (const DataStructure &other) const;


    // create label data from input
    virtual vector<float> getDataBatch(vector<float> input) {return vector<float>();};
};


/*
 * Function DataStructure class
 */
class FunctionDataStructure : public DataStructure
{
  public:
    string function;

    FunctionDataStructure();

    virtual vector<float> getDataBatch(vector<float> input) override;
};


#endif //DATASTRUCTURE_H
