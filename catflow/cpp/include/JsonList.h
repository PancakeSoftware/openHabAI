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
#include <boost/static_assert.hpp>
#include "ApiJsonObject.h"
#include "ApiMessage.h"
#include "ApiProcessible.h"
using Json = nlohmann::json;
using namespace std;

class __JsonList {};

template<class T>
/**
 * JsonList class
 * elements updatable via json (get, getAll, create)
 * @param T extends JsonObject
 * @note if derived objects of T are inserted into JsonList, T needs a virtual destructor because on destruction JsonList also deletes all its items
 */
class JsonList : public ApiProcessible, public __JsonList, protected Log
{
    BOOST_STATIC_ASSERT_MSG(is_base_of<ApiProcessible, T>::value, "JsonList<T> T has to be derivative of ApiProcessible to provide necessary functionality"); // @TODO list for every type

  public:
    string folder;
    string entryFileName;
    int defaultIdAutoIncrement = 0;

    JsonList();
    JsonList(int &idAutoIncrement);

    ~JsonList();

    /**
     * progress first route-object in route list
     * @param route route list, defines something similar to rest url, in this example you want to access
     * @param what  defines action to perform, allowed values depend on route
     * @param data  contains data that is necessary to perform action
     */
    virtual ApiRespond* processApi(ApiRequest request);


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

    void restore() override;
    void store() override;
    void remove() override;
    void setRoute(ApiMessageRoute route) override;

  private:
    int &idAutoIncrement;
    function<T *(Json params)> createItemFunc = [](Json params) -> T *
    {
      T *t = new T();
      t->fromJson(params);
      return t;
    };

};


#include "../source/JsonList.tpp"
#endif //OPENHABAI_JSONLIST_H
