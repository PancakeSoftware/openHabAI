/* 
 * File:   DataStructure.h
 * Author: Joshua Johannson
 *
  */
#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include <json.hpp>
#include <string>
#include <ApiJsonObject.h>
#include <JsonList.h>
#include <ApiRoute.h>
#include <util/util.h>
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
    map<int, string> inputNames;
    map<int, string> outputNames;

    ParameterChart dataChart;
    JsonList<NeuralNetwork> networks;

    /* Json keys */
    void params() override;

    DataStructure();

    /**
     * because this is type and its child classes, of list
     * a virtual destructor is needed
     */
    virtual ~DataStructure() = default;

    static DataStructure * create(Json params); // create right structure type by param 'type'

    inline bool operator == (int other) const;
    inline bool operator == (const DataStructure &other) const;


    /**
     * get data-batch (list of (input Values - output Values) pairs)
     * @param indexBegin first data-record index
     * @param indexEnd   last  data-record index
     * @return [ [one data-record: [inputValues: ], [outputValues]],
     *          ...]
     * @note less effective because copy is made
     */
    virtual vector<pair<vector<float>, vector<float>>> getDataBatch(int indexBegin, int indexEnd) {
        return vector<pair<vector<float>, vector<float>>>(data.begin()+indexBegin, data.begin()+indexEnd);
    };

    /**
     * get data-batch (list of (input Values - output Values) pairs)
     * @return [ [one data-record: [inputValues: ], [outputValues]],
     *          ...]
     */
    virtual vector<pair<vector<float>, vector<float>>>& getDataBatch() {
      return data;
    };

    /**
     * get amount of data-records
     * -> index can be passed into getDataBatch(index)
     * @return max index of data-record
     */
    virtual int getDataBatchIndices()
    { return data.size(); };

  protected:
    vector<pair<vector<float>, vector<float>>> data;
};


/*
 * Function DataStructure class
 */
class FunctionDataStructure : public DataStructure
{
  public:
    string function;
    vector<Range> inputRanges;

    /* Json keys */
    void params() override { DataStructure::params();
        param("function", function);
        param("inputRanges", inputRanges);
    }

    FunctionDataStructure();
    void onParamsChanged(vector<string> params) override;

};


#endif //DATASTRUCTURE_H
