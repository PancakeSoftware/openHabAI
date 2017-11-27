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
    JsonObject(Json params);

    /**
     * @return configured class attributes as json
     * @see JsonObject::setJsonParams
     */
    virtual Json toJson();
    /**
     * @param params set configured class attributes to values in params
     * @see JsonObject::setJsonParams
     */
    virtual void fromJson(Json params);

    /**
     * store toJson output in file
     * @param path
     * @param fileName
     * @return true if successful
     */

    virtual bool save(string path, string fileName);
    /**
     * load json from file and apply to object
     * @param path
     * @param fileName
     * @return true if successful
     */
    virtual bool load(string path, string fileName);

    /**
     * reserved
     * @TODO add doc
     */
    virtual void loadChilds();

    /**
     * link class attributes to json keys
     * format: map of {"JSON_KEY", POINTER_TO_ATTRIBUTE}
     * @attention supported data types: int, float, double, bool
     * @param params expl.: {{"value1", &i},{"value2", &s}}
     */
    void setJsonParams(map<string, boost::any>  params);

  private:
    map<string, boost::any> paramPointers;
};

#endif //OPENHABAI_JSONOBJECT_H
