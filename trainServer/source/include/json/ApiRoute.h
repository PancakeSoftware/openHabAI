/* 
 * File:   ApiRoute.h
 * Author: Joshua Johannson
 *
  */
#ifndef OPENHABAI_APIROUTE_H
#define OPENHABAI_APIROUTE_H

#include <json.hpp>
#include <string>
#include <list>
#include <json/ApiMessage.h>
#include <util/Log.h>
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
    virtual ApiRespond* processApi(ApiRequest request) = 0;
};

/*
 * ApiRoute class
 * represents one route object in route list
 * expl.: here {"Component-A":   "entityID-A"}
 * "route": [
 *   {"Component-A":   "entityID-A"},
 *   {"Sub-Component": "entityID-B"},
 * ]
 */
class ApiRoute : public ApiProcessible, protected Log
{
  public:

    ApiRoute();

    /**
     * @param subRoutes set 'static' sub routes
     * @see setSubRoutes(vector<ApiRoute*>& routes)
     */
    ApiRoute(map<string, ApiProcessible*> subRoutes);


    /**
     * progress first route-object in route list
     * @param route route list, defines something similar to rest url, in this example you want to access
     * @param what  defines action to perform, allowed values depend on route
     * @param data  contains data that is necessary to perform action
     */
    virtual ApiRespond* processApi(ApiRequest request);

    /**
     * set 'static' sub routes
     * @param routes pair.first = the ApiRoute object,
     *               pair.second = name of route
     */
    void setSubRoutes(map<string, ApiProcessible*> routes);

  private:
    map<string, ApiProcessible*> routes;
};

#endif //OPENHABAI_APIROUTE_H
