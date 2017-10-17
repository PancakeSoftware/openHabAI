/* 
 * File:   JsonObject.h
 * Author: Joshua Johannson
 *
  */
#ifndef OPENHABAI_JSONOBJECT_H
#define OPENHABAI_JSONOBJECT_H

#include <util/Log.h>
#include <json.hpp>
#include <string>
#include <list>
#include <boost/any.hpp>
using Json = nlohmann::json;
using namespace std;

/*
 * JsonObject class
 * bind variables to json keys
 * supported data types: int, float, double, bool
 */
class JsonObject : protected Log
{
  public:
    JsonObject();
    virtual Json toJson();
    virtual void fromJson(Json params);

    virtual bool save(string path, string fileName);
    virtual bool load(string path, string fileName);
    virtual void loadChilds();

    void setJsonParams(map<string, boost::any>  params);

  private:
    map<string, boost::any> paramPointers;
};

#endif //OPENHABAI_JSONOBJECT_H
