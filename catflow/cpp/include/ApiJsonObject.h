/* 
 * File:   ApiJsonObject.h
 * Author: Pancake Software
 */
#ifndef OPENHABAI_APIJSONOBJECT_H
#define OPENHABAI_APIJSONOBJECT_H

#include <stdlib.h>
using namespace std;


#include <util/Log.h>
#include <json.hpp>
#include <string>
#include <list>
#include <memory>
#include <boost/any.hpp>
#include <unordered_map>
#include "ApiProcessible.h"
#include "Util.h"
#include "ApiSubscribable.h"
#include <stdarg.h>
#include "JsonObject.h"

/*
 * JsonObject class
 * bind variables to json keys
 * supported data types: int, float, double, bool
 */
class ApiJsonObject : protected virtual Log, public virtual ApiSubscribable, public virtual JsonObject
{
  public:
    ApiJsonObject();
    ApiJsonObject(Json params);


    /**
     * set configured class attributes to values in params
     * , save changes and send updates to subscribers
     * @see JsonObject::params()
     * @return vector of changed param names
     */
    vector<string> fromJson(Json params) override;

    /**
     * set configured class attributes to values in params
     * , save changes and send updates to subscribers
     * @param skipSendUpdateTo not send update to this client
     * @see JsonObject::params()
     * @return vector of changed param names
     */
    vector<string> fromJson(Json params, Client &skipSendUpdateTo);


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
     * send changed object params to all subscribers
     * @param param... changed json object params
     */
    void notifyParamsChanged(vector<string> param);
    void notifyParamsChanged(string param, ...);


    ApiRespond *processApi(ApiRequest request) override;
    void restore() override;
    void store() override;
    void remove() override;
    void storeMe() override;
    void setRoute(ApiMessageRoute route) override;

  private:
    template <typename T>
    vector<T> flatten(T t)
    {
        vector<T> v;
        v.push_back(t);
        return v;
    }

    template<typename T, typename... Args>
    vector<T> flatten(T t, Args... args) // recursive variadic function
    {
        vector<T> v = {t};
        vector<T> before = flatten(args...);
        v.insert(before.begin(), before.end());
        return v;
    }
};

/**
 * helper
 */




#endif OPENHABAI_APIJSONOBJECT_H
