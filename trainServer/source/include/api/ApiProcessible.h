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
#include <api/ApiMessage.h>
#include <boost/optional.hpp>
using Json = nlohmann::json;
using namespace std;

typedef vector<pair<string, string>> RoutePath;

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
        return el->second(request);
      else
        return nullptr;
    };

    /**
     * restore saved state of self and subObjects
     */
    virtual void restore() {};

    /**
     * store state of self and subObjects
     */
    virtual void store() {};

    /**
     * store state of self and subObjects
     */
    virtual void storeMe() {};

    /*
     * set full path to store this
     * this calls setStorePath of subObject recursively
     * example: {
     *   {"Component-A":   "entityID-A"},
     *   {"Sub-Component": "entityID-B"},
     * }
     */
    virtual void setStorePath(RoutePath path)
    {
      route = path;
      storePathString = getPath();
    }

  protected:
    boost::optional<RoutePath> route;
    string storePathString;

    string getPath() {
      if (!route.is_initialized())
        return "";

      string path = "";
      for (auto r : route.get()) {
        path += r.first + "/";
        if (r.second != "")
          path +=  r.second + "/";
      }

      storePathString = path;
      return path;
    }

    void addWhat(string what, function<ApiRespond*(ApiRequest request)> action)
    {processWhats.insert({what, action});};

  private:
    map<string, function<ApiRespond*(ApiRequest request)>> processWhats;
};



/*
 * from and to json for RoutePath
 */
/*
using nlohmann::json;
void to_json(json& j, const RoutePath& p) {
  j = Json{};
  for (auto item : p)
    j.push_back({{item.first, item.second}});
}*/

/*
  void from_json(const Json& j, RoutePath& p) {
    for (auto item : j)
      p.push_back({item.begin().key(), item.begin().value()});
  }
  */


#endif //OPENHABAI_APIPROCESSIBLE_H
