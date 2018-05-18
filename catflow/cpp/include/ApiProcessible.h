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
#include "ApiMessage.h"
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
     * @param request.route defines something similar to rest url, in this example you want to access
     * @param request.what  defines action to perform, allowed values depend on route
     * @param request.data  contains data that is necessary to perform action
     */
    virtual ApiRespond* processApi(ApiRequest request)
    {
      auto el = processWhats.find(request.what);
      if (el != processWhats.end())
        return el->second(request);
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
     * removes saved data of self and subObjects
     */
    virtual void remove() {};

    /**
     * store state of self
     * @TODO merge with store()
     */
    virtual void storeMe() {};

    /*
     * set full path to store this
     * this calls setStorePath of subObject recursively
     * example: /students/0/courses/0
     */
    virtual void setRoute(ApiMessageRoute route)
    {
      this->route = route;
      routeString = route.toString();
    }

    /**
     * set full path to store this and defines routes of all children
     * @example ./my/file/path/to/store/root/of/api/
     * @param path has to end with '/'
     */
    virtual void setStorePath(const string path) {
      ApiMessageRoute myRoute;
      myRoute.pathPrefix = path;
      this->setRoute(myRoute);
    }


  protected:
    boost::optional<ApiMessageRoute> route;
    string routeString;

    void addAction(string what, function<ApiRespond *(ApiRequest request)> action)
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
