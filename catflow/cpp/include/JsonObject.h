/*
 * File:   JsonObject.h
 * Author: Pancake Software
 */

#ifndef OPENHABAI_JSONOBJECT_H
#define OPENHABAI_JSONOBJECT_H

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
#include <cstdarg>
using Json = nlohmann::json;
using namespace std;



class AJsonParam
{
  public:
    virtual Json toJson() const {};
    virtual void fromJson(Json j) {};
    virtual bool isPrimitive(){ return true; };

    /**
     * called when param change, before new value is applied to class member
     * -> you can compare old and new value
     */
    void onChange(function<void ()> callback)
    {this->onChanceFunc = callback;};

    /**
     * called after params change, after new value is applied to class member
     */
    void onChanged(function<void ()> callback)
    {this->onChancedFunc = callback;};

    /**
     * make param read only
     */
    AJsonParam& readOnly() {
      _writable = false;
      return *this;
    };

    /**
     * make param write only
     */
    AJsonParam& writeOnly() {
      _readable = false;
      return *this;
    };

    /**
     * make param non save changes in .json file
     */
    AJsonParam& nonSave() {
      _savable = false;
      return *this;
    };

    /**
     * make param hidden, value only visible on changes
     * @warning not implemented
     */
    AJsonParam& hidden() {
      _hidden = true;
      return *this;
    };


    function<void ()> onChanceFunc = nullptr;
    function<void ()> onChancedFunc = nullptr;
    bool _readable = true;
    bool _writable = true;
    bool _savable = true;
    bool _hidden = false;
};

namespace internal
{
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

      void fromJson(Json j) override;

      ~JsonParam() {
        //cout << "[JsonParam] ("<< key << ": " << ") " <<"delete: "<< counter << endl;
      }

      bool isPrimitive() override;
  };

  template <class T>
  class JsonParamReadOnly: public AJsonParam{
    public:
      T* valuePtr;
      string key;
      JsonParamReadOnly(T &value, string key) {
        this->valuePtr = &value;
        this->key = key;
        readOnly();
      }

      Json toJson() const override {
        return Json(*valuePtr);
      }

      void fromJson(Json j) override {
        throw JsonObjectException("can't set '"+key+"' because it is readonly");
      }

      bool isPrimitive() override;
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
     * @param onlySavableParams skip all params that are marked as nonSave()
     * @param skipHiddenParams skip all params that are maked as hidden()
     * @see JsonObject::params()
     */
    virtual Json toJson(bool onlySavableParams = false, bool skipHiddenParams = true) const {
      /* refresh param pointers
       * this is very inefficient but necessary (rebuild whole param list):
       * when the object is copied all param pointers have to be redefined (change pointer address) */
      refreshParams();

      Json json;
      for (auto &paramPtr: paramPointers) {
        const auto& key = paramPtr.first;
        const auto& memberPtr = paramPtr.second;
        if ((!onlySavableParams || memberPtr->_savable) &&
            (!skipHiddenParams  || !memberPtr->_hidden) &&
            memberPtr->_readable)
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
          if (memberPtr->_readable)
            json[key] = memberPtr->toJson();
      }
      return json;
    }


    /**
     * @param params set configured class attributes to values in params
     * @see JsonObject::params()
     * @return vector of changed param names
     */
    virtual vector<string> fromJson(Json params, bool  catchParameterErrors = false) {
      /* refresh param pointers
       * this is very inefficient but necessary (rebuild whole param list):
       * when the object is copied all param pointers have to be redefined (change pointer address) */
      refreshParams();
      vector<string> willChange;
      vector<string> changed;

      // filter existing keys   @TODO less redundant
      for (auto &paramPtr: paramPointers) {
        const auto &key = paramPtr.first;
        const auto &memberPtr = paramPtr.second;
        if (params.find(key) == params.end() && memberPtr->isPrimitive())
          continue;
        willChange.push_back(key);

        if (memberPtr->onChanceFunc != nullptr)
          memberPtr->onChanceFunc();
      }
      onParamsChange(willChange);


      for (auto &paramPtr: paramPointers) {
        const auto &key = paramPtr.first;
        const auto &memberPtr = paramPtr.second;
        if (params.find(key) == params.end())
          continue;

        try {
          if (!memberPtr->_writable)
            throw JsonObjectException("can't set '"+key+"' because it is readonly");

          //cout << "=fromJson= param " << key << ": " << params[key].dump() << endl;
          memberPtr->fromJson(params[key]);
          if (memberPtr->isPrimitive()) {
            changed.push_back(key);
            if (memberPtr->onChancedFunc != nullptr)
              memberPtr->onChancedFunc();
          }
        }
        catch (Json::type_error &e) {
          l.err("can't set jsonObject key '" + key +"' : " + e.what());
          if (!catchParameterErrors)
            throw JsonObjectException("can't set jsonObject key '" + key +"' because of wrong type : " + e.what());
        } catch (JsonObjectException &e) {
          if (!catchParameterErrors)
            throw JsonObjectException("can't set jsonObject key '" + key +"' because of JsonObjectException : " + e.what());
          else
            l.err("can't set jsonObject key '" + key +"' because of JsonObjectException : " + e.what());
        }
      }

      onParamsChanged(changed);
      return changed;
    };

    /**
     * links json key with class member's value
     * @warning use this function only inside your implementation of defineParams() !
     * @see defineParams()
     */
    template <class T>
    AJsonParam& param(string key, T &value) {
      auto param = make_shared<internal::JsonParam<T>>(value, key);
      paramPointers.emplace(key, param); // unique_ptr -> deletes JsonParam when JsonObject is deleted
      return *param;
    }

    /**
     * links json key with class member's value
     * @note this param is only readable
     * @warning use this function only inside your implementation of defineParams() !
     * @see defineParams()
     */
    template <class T>
    AJsonParam& paramReadOnly(string key, T &value) {
      auto param = make_shared<internal::JsonParamReadOnly<T>>(value, key);
      paramPointers.emplace(key, param); // unique_ptr -> deletes JsonParam when JsonObject is deleted
      return *param;
    }

    /**
     * links json key with getter and setter function
     * @warning use this function only inside your implementation of defineParams() !
     * @see defineParams()
     */
    AJsonParam& paramWithFunction(string key, function<void (Json)> onSet, function<Json ()> onGet) {
      auto param = make_shared<internal::JsonParamFunction>(onSet, onGet, key);
      paramPointers.emplace(key, param); // unique_ptr -> deletes JsonParam when JsonObject is deleted
      return *param;
    }

    /**
     * define json params
     * use function param() only inside this function!
     */
    virtual void params() {
    };


    /**
     * called when params change, before new value is applied to class member
     * -> you can compare old and new value
     * @param params the changed params
     */
    virtual void onParamsChange(vector<string> params) {};

    /**
     * called after params change, after new value is applied to class member
     * @param params the changed params
     */
    virtual void onParamsChanged(vector<string> params) {};


    ~JsonObject() {
      paramPointers.clear();
    }

  private:
    map<string, shared_ptr<AJsonParam>> paramPointers;
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

#include "../source/JsonObject.tpp"
#endif //OPENHABAI_JSONOBJECT_H
