/* 
 * File:   ApiProcessible.h
 * Author: Joshua Johannson
 *
  */
#ifndef OPENHABAI_APIPROCESSIBLE_H
#define OPENHABAI_APIPROCESSIBLE_H

#include <json.hpp>
#include <string>
#include <list>
#include <json/ApiMessage.h>
#include <boost/optional.hpp>
using Json = nlohmann::json;
using namespace std;

/*
 * Apiprocessible class
 * object than is able to process api requests
 * -> via processApi
 */
class ApiProcessible
{
  public:
    /**
     * progress first route-object in route list
     * @param request.route route list, defines something similar to rest url, in this example you want to access
     * @param request.what  defines action to perform, allowed values depend on route
     * @param request.data  contains data that is necessary to perform action
     */
    virtual ApiRespond* processApi(ApiRequest request)
    {
      auto el = processWhats.find(request.what);
      if (el != processWhats.end())
        el->second(request);
    };

    /**
     * restore saved state of self and subObjects
     */
    virtual void restore() {};

    /**
     * store state of self and subObjects
     */
    virtual void store() {};

    /*
     * set full path to store this
     * this calls setStorePath of subObject recursively
     */
    virtual void setStorePath(string path)
    {storePath = path;}

  protected:
    boost::optional<string> storePath;

    void addWhat(string what, function<ApiRespond*(ApiRequest request)> action)
    {processWhats.insert({what, action});};

  private:
    map<string, function<ApiRespond*(ApiRequest request)>> processWhats;
};

#endif //OPENHABAI_APIPROCESSIBLE_H
