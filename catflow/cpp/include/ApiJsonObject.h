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
#include <memory>
#include <boost/any.hpp>
#include <unordered_map>
#include "ApiProcessible.h"
#include "Util.h"
#include "ApiSubscribable.h"
#include <stdarg.h>
using Json = nlohmann::json;
using namespace std;


namespace internal
{

class AJsonParam
{
  public:
    virtual Json toJson() const {};
    virtual void fromJson(Json j) {};
};

template <class T>
class JsonParam: public AJsonParam{
  public:
    T* valuePtr;
    string key;

    JsonParam(T &value, string key) {
      this->valuePtr = &value;
      this->key = key;

      // cout << "[JsonParam] ("<< key << ": " << ") " <<"create: "<< counter << endl;
    }

    Json toJson() const override {
      //cout << "[JsonParam] toJson() "<< key << ": " << static_cast<void*>(valuePtr) << endl;
      return Json(*valuePtr);
    }

    void fromJson(Json j) override {
      *valuePtr = j.get<T>();
    }

    ~JsonParam() {
      //cout << "[JsonParam] ("<< key << ": " << ") " <<"delete: "<< counter << endl;
    }
};

template <class T>
class JsonParamReadOnly: public AJsonParam{
  public:
    T* valuePtr;
    JsonParamReadOnly(T &value, string key)
    {this->valuePtr = &value;}

    Json toJson() const override {
      return Json(*valuePtr);
    }

    void fromJson(Json j) override
    {}
};


class JsonParamFunction: public AJsonParam{
  public:
    function<void (Json)> onSet;
    function<Json ()> onGet;

    JsonParamFunction(function<void (Json)> onSet, function<Json ()> onGet, string key)
    {this->onGet = onGet; this->onSet = onSet;}

    Json toJson() const override {
      return onGet();
    }

    void fromJson(Json j) override
    {
      onSet(j);
    }
};

}

class JsonObject
{
  public:
    /**
     * @return configured class attributes as json
     * @see JsonObject::params()
     */
    virtual Json toJson() const {
      /* refresh param pointers
       * this is very inefficient but necessary (rebuild whole param list):
       * when the object is copied all param pointers have to be redefined (change pointer address) */
      refreshParams();

      Json json;
      for (auto &paramPtr: paramPointers) {
        const auto& key = paramPtr.first;
        const auto& memberPtr = paramPtr.second;
        json[key] = memberPtr->toJson();
      }
      return json;
    };

    /**
     * @return configured class attributes as json, but only params that match a string in params parameter
     * @see JsonObject::params()
     * @param params only this params converted to json
     */
    virtual Json toJson(vector<string> params) const {
        refreshParams();

        Json json;
        for (auto &paramPtr: paramPointers) {
            const auto& key = paramPtr.first;
            const auto& memberPtr = paramPtr.second;
            if (find(params.begin(), params.end(), key) != params.end()) // if key in params
                json[key] = memberPtr->toJson();
        }
        return json;
    }


    /**
     * @param params set configured class attributes to values in params
     * @see JsonObject::params()
     * @return vector of changed param names
     */
    virtual vector<string> fromJson(Json params) {
      /* refresh param pointers
       * this is very inefficient but necessary (rebuild whole param list):
       * when the object is copied all param pointers have to be redefined (change pointer address) */
      refreshParams();
      vector<string> changed;

      for (auto &paramPtr: paramPointers) {
        const auto &key = paramPtr.first;
        const auto &memberPtr = paramPtr.second;
        if (params.find(key) == params.end())
          continue;

        try {
          //cout << "=fromJson= param " << key << ": " << params[key].dump() << endl;
          memberPtr->fromJson(params[key]);
          changed.push_back(key);
        } catch (Json::type_error &e) {
          l.err("can't set jsonObject key '" + key +"' : " + e.what());
          throw JsonObjectException("can't set jsonObject key '" + key +"' because of wrong type : " + e.what());
        }
      }
      return changed;
    };

    /**
     * links json key with class member's value
     * @warning use this function only inside your implementation of defineParams() !
     * @see defineParams()
     */
    template <class T>
    void param(string key, T &value) {
        paramPointers.emplace(key, make_shared<internal::JsonParam<T>>(value, key)); // unique_ptr -> deletes JsonParam when JsonObject is deleted
    }

    /**
     * links json key with class member's value
     * @note this param is only readable
     * @warning use this function only inside your implementation of defineParams() !
     * @see defineParams()
     */
    template <class T>
    void paramReadOnly(string key, T &value) {
      paramPointers.emplace(key, make_shared<internal::JsonParamReadOnly<T>>(value, key)); // unique_ptr -> deletes JsonParam when JsonObject is deleted
    }

    /**
     * links json key with getter and setter function
     * @warning use this function only inside your implementation of defineParams() !
     * @see defineParams()
     */
    void paramWithFunction(string key, function<void (Json)> onSet, function<Json ()> onGet) {
      paramPointers.emplace(key, make_shared<internal::JsonParamFunction>(onSet, onGet, key)); // unique_ptr -> deletes JsonParam when JsonObject is deleted
    }

    /**
     * define json params
     * use function param() only inside this function!
     */
    virtual void params() {
    };


    ~JsonObject() {
      paramPointers.clear();
    }

  private:
    map<string, shared_ptr<internal::AJsonParam>> paramPointers;
    static Log l;

    void refreshParams() const {
      auto t = const_cast<JsonObject*>( this );
      t->paramPointers.clear();
      t->params();
    }
};


static void to_json(Json& j, const JsonObject& p) {
    j = p.toJson();
}

static void from_json(const Json& j, JsonObject& p) {
    p.fromJson(j);
}


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
    vector<string> fromJson(Json params, Client skipSendUpdateTo);


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




#endif //OPENHABAI_JSONOBJECT_H
