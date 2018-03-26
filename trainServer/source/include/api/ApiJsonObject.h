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
using Json = nlohmann::json;
using namespace std;


namespace internal
{

class AJsonParam
{
  public:
    virtual Json toJson() const {};
    virtual void fromJson(Json j) const {};
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

    void fromJson(Json j) const override {
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

    void fromJson(Json j) const override
    {}
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
     * @param params set configured class attributes to values in params
     * @see JsonObject::params()
     */
    virtual void fromJson(Json params) {
      /* refresh param pointers
       * this is very inefficient but necessary (rebuild whole param list):
       * when the object is copied all param pointers have to be redefined (change pointer address) */
      refreshParams();

      for (auto &paramPtr: paramPointers) {
        const auto &key = paramPtr.first;
        const auto &memberPtr = paramPtr.second;
        if (params.find(key) == params.end())
          continue;

        try {
          //cout << "=fromJson= param " << key << ": " << params[key].dump() << endl;
          memberPtr->fromJson(params[key]);
        } catch (exception &e) {
          l.err("can't set jsonObject key '" + key +"' : " + e.what());
        }
      }
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
class ApiJsonObject : protected virtual Log, public virtual ApiProcessible, public virtual JsonObject
{
  public:
    ApiJsonObject();
    ApiJsonObject(Json params);


    void fromJson(Json params) override;

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


    ApiRespond *processApi(ApiRequest request) override;
    void restore() override;
    void store() override;
    void storeMe() override;
    void setStorePath(RoutePath path) override;
};

#endif //OPENHABAI_JSONOBJECT_H
