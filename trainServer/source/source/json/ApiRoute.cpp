/* 
 * File:   ApiRoute.h
 * Author: Joshua Johannson
 *
  */
#include <json/JsonObject.h>
#include "json/ApiRoute.h"
#include <json/JsonList.h>


ApiRoute::ApiRoute() : Log("APIR")
{}

ApiRoute::ApiRoute(map<string, ApiProcessible*> subRoutes) : ApiRoute()
{
  setSubRoutes(subRoutes);
}

ApiRespond* ApiRoute::processApi(ApiRequest request)
{
  // if route == empty, this is target
  if (request.route.size() == 0) {
    //warn("what: '"+ request.what +"' can not be processed by ApiRoute, route: " + request.route.dump(2));
    return nullptr;//new ApiRespondError("what: '"+ request.what +"' can not be processed by ApiRoute", request);
  }

  //info("route "+ to_string(request.route.size()) +": " + request.route.dump(2));

  string component = (*request.route.begin()).begin().key();
  string entityId  = (*request.route.begin()).begin().value();

  // get route
  if (routes.find(component) == routes.end())
    return new ApiRespondError("subRoute: '"+ component +"' dose not exist", request);
  else
  {
    ApiProcessible *subRoute = routes.find(component)->second;
    // pop top route-element if not needed by subRoute (JsonList deletes it itself)
    if (!dynamic_cast<__JsonList*>(subRoute))
    {
      //info("remove route");
      request.route.erase(request.route.begin());
    }

    return subRoute->processApi(request);
  }

}

void ApiRoute::setSubRoutes(map<string, ApiProcessible*> routes)
{
  this->routes = routes;
}

void ApiRoute::restore()
{
  for (auto route : routes)
    route.second->restore();
}

void ApiRoute::setStorePath(string path)
{
  ApiProcessible::setStorePath(path);
  for (auto route : routes)
    route.second->setStorePath(this->storePath.get() + "/" + route.first);
}

void ApiRoute::store()
{
  ApiProcessible::store();
  for (auto route : routes)
    route.second->store();
}
