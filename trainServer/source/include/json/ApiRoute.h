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
#include <json/JsonList.h>
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
class ApiRoute
{
  public:

    ApiRoute();

    /**
     * @param subRoutes set 'static' sub routes
     * @see setSubRoutes(vector<ApiRoute*>& routes)
     */
    ApiRoute(vector<ApiRoute*> subRoutes);


    /**
     * progress first route-object in route list
     * @param route route list, defines something similar to rest url, in this example you want to access
     * @param what  defines action to perform, allowed values depend on route
     * @param data  contains data that is necessary to perform action
     */
    virtual void progressApi(Json route, string what, Json data);

    /**
     * set 'static' sub routes
     * @param routes
     */
    void setSubRoutes(vector<ApiRoute*>& routes);

  private:
    vector<ApiRoute*>* routes;
};


/*
 * ApiRouteWithEntitys class
 * represents one route object in route list
 * expl.: here {"Component-A":   "entityID-A"}
 * "route": [
 *   {"Component-A":   "entityID-A"},
 *   {"Sub-Component": "entityID-B"},
 * ]
 * also contains JsonList as entity representation
 */
template <class T>
class ApiRouteWithEntities : public ApiRoute
{
  public:

    /**
     * @param jsonList set responsible object to progess add, set,... actions
     */
    ApiRouteWithEntities(JsonList<T>& jsonList)
    {
        this->jsonList = &jsonList;
    }

    /**
     * @param jsonList set responsible object to progess add, set,... actions
     * @param subRoutes set 'static' sub routes
     */
    ApiRouteWithEntities(JsonList<T>& jsonList, vector<ApiRoute*>& subRoutes) :
        ApiRouteWithEntities(jsonList)  {}

  private:
    JsonList<T>* jsonList;
};

#endif //OPENHABAI_APIROUTE_H
