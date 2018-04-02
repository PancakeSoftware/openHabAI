/* 
 * File:   ApiRoute.h
 * Author: Joshua Johannson
 *
  */
#include "api/ApiJsonObject.h"
#include "api/ApiRoute.h"


ApiRoute::ApiRoute() : Log("ApiRoute")
{}

ApiRoute::ApiRoute(map<string, ApiProcessible*> subRoutes) : ApiRoute()
{
  setSubRoutes(subRoutes);
}

ApiRespond* ApiRoute::processApi(ApiRequest request)
{
  // if route == empty, this is target
  if (request.route.isEmpty()) {
    //warn("what: '"+ request.what +"' can not be processed by ApiRoute, route: " + request.route.dump(2));
    return nullptr;//new ApiRespondError("what: '"+ request.what +"' can not be processed by ApiRoute", request);
  }

  //info("route "+ to_string(request.route.size()) +": " + request.route.dump(2));

  string component = request.route.pop();

  // get route
  if (routes.find(component) == routes.end())
    return new ApiRespondError("subRoute: '"+ component +"' dose not exist", request);
  else {
    ApiProcessible *subRoute = routes.find(component)->second;
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

void ApiRoute::setRoute(ApiMessageRoute route)
{
  ApiProcessible::setRoute(route);
  //info("my path: " + storePathString.get_value_or("??"));
  for (auto child : routes) {
    ApiMessageRoute n = route;
    n.push(child.first);
    child.second->setRoute(n);
  }
}

void ApiRoute::store()
{
  ApiProcessible::store();
  for (auto route : routes)
    route.second->store();
}
