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
#include <api/ApiMessage.h>
#include <util/Log.h>
#include <boost/optional.hpp>
#include "api/JsonList.h"
#include "api/ApiJsonObject.h"
#include "ApiProcessible.h"
using Json = nlohmann::json;
using namespace std;

/*
 * ApiRoute class
 * represents one route object in route list
 * expl.: here {"Component-A":   "entityID-A"}
 * "route": [
 *   {"Component-A":   "entityID-A"},
 *   {"Sub-Component": "entityID-B"},
 * ]
 */
class ApiRoute : public virtual ApiProcessible, protected virtual Log
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

    void restore() override;
    void store() override;
    void setStorePath(RoutePath path) override;

  private:
    map<string, ApiProcessible*> routes;
};


class ApiJsonObject;
class ApiRouteJson: public ApiRoute, public ApiJsonObject
{
  public:

    ApiRouteJson() : ApiRoute(){}

    /**
     * @param subRoutes set 'static' sub routes
     * @see setSubRoutes(vector<ApiRoute*>& routes)
     */
    ApiRouteJson(map<string, ApiProcessible*> subRoutes) : ApiRoute(subRoutes)  {}

    ApiRespond *processApi(ApiRequest request) override
    {
      auto resp =  ApiRoute::processApi(request);
      if (resp == nullptr && request.route.size() <= 0)
        return ApiJsonObject::processApi(request);
      else
        return resp;
    }

    void restore() override
    {
      ApiJsonObject::restore();
      ApiRoute::restore();
    }

    void store() override
    {
      ApiJsonObject::store();
      ApiRoute::store();
    }

    void storeMe() override
    {
      ApiJsonObject::storeMe();
    }

    void setStorePath(RoutePath path) override
    {
      ApiJsonObject::setStorePath(path);
      ApiRoute::setStorePath(path);
    }
};

#endif //OPENHABAI_APIROUTE_H
