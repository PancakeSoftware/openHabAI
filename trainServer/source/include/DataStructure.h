/* 
 * File:   DataStructure.h
 * Author: Joshua Johannson
 *
  */
#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include <json.hpp>
#include <string>
#include <JsonList.h>
using namespace std;


/*
 * DataStructure class
 * provides/generates training data
 */
class DataStructure : public JsonListItem
{
  public:
    enum TYPE {
        TYPE_FUNCTION,
        TYPE_OPENHAB
    } type_;
    string name;

    static int idIncrement;
    int networkIdIncrement = 0;
    JsonList networks;

    DataStructure();
    DataStructure(Json params);
    DataStructure(string path, string filename);
    static DataStructure * create(Json params); // create right structure type by param 'type'

    string type();
    void type(string type);

    virtual Json toJson();
    virtual void fromJson(Json params);

    inline bool operator == (int other) const;
    inline bool operator == (const DataStructure &other) const;

    virtual void loadChilds() override;

    // create label data from input
    virtual vector<float> getDataBatch(vector<float> input) = 0;
};


/*
 * Function DataStructure class
 */
class FunctionStructure : public DataStructure
{
  public:
    string function;

    FunctionStructure(Json params);

    virtual vector<float> getDataBatch(vector<float> input) override;
    virtual Json toJson() override;
    virtual void fromJson(Json params) override;
};


#endif //DATASTRUCTURE_H
