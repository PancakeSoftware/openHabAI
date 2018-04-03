/* 
 * File:   DataStructure.h
 * Author: Joshua Johannson
 *
  */
#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include <json.hpp>
#include <string>
#include <api/ApiJsonObject.h>
#include <api/JsonList.h>
#include <api/ApiRoute.h>
#include "NeuralNetwork.h"
using namespace std;


/*
 * DataStructure class
 * provides/generates training data
 */
class DataStructure : public ApiRouteJson
{
  public:
    int id;
    string name;
    string type;
    ParameterChart dataChart;

    /* Json keys */
    void params() override { ApiRouteJson::params();
        param("name", name);
        param("type", type);
        param("id", id);
    }

    JsonList<NeuralNetwork> networks;

    DataStructure();

    /**
     * because this is type and its child classes, of list
     * a virtual destructor is needed
     */
    virtual ~DataStructure() = default;

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

    /* Json keys */
    void params() override { DataStructure::params();
        param("function", function);
    }

    FunctionDataStructure();

    //~FunctionDataStructure(){
    //}

    virtual vector<float> getDataBatch(vector<float> input) override;
};


#endif //DATASTRUCTURE_H
