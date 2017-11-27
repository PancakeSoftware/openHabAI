/* 
 * File:   JsonList.h
 * Author: Joshua Johannson
 *
 */
#ifndef OPENHABAI_JSONLIST_H
#define OPENHABAI_JSONLIST_H

#include <json.hpp>
#include <string>
#include <list>
#include <json/JsonObject.h>
#include <json/ApiMessage.h>
#include <boost/static_assert.hpp>
using Json = nlohmann::json;
using namespace std;


template<class T>
/**
 * JsonList class
 * elements updatable via json (get, getAll, create)
 * @param T extends JsonObject 
 */
class JsonList : private Log
{
    BOOST_STATIC_ASSERT((is_base_of<JsonObject, T>::value));

  public:
    string folder;
    string entryFileName;

    JsonList(
        int &idAutoIncrement,
        string folder,
        string entryFileName);

    /**
     * progress first route-object in route list
     * @param route route list, defines something similar to rest url, in this example you want to access
     * @param what  defines action to perform, allowed values depend on route
     * @param data  contains data that is necessary to perform action
     */
    virtual Json progressApi(ApiRequest &request);

    /**
     * loads all list elements from saved .json files
     */
    bool loadAll();


    /**
     * get by id
     * @param id
     * @return NULL if not found
     */
    T &get(int id);

    /**
     * @return amount of items in list
     */
    int length();

    /**
     * items list
     * @param first id
     * @param second T
     */
    map<int, T *> items;

    /**
     * set function to create item instance
     * @param createItemFunc
     */
    void setCreateItemFunction(function<T *(Json params)> createItemFunc);


  private:
    int &idAutoIncrement;
    function<T *(Json params)> createItemFunc = [](Json params) -> T *
    {
      T *t = new T();
      t->fromJson(params);
      return t;
    };

    void progressCommand(Json command); // progress command -> send request
};


#include "../../source/json/JsonList.tpp"
#endif //OPENHABAI_JSONLIST_H
