/* 
 * File:   ApiRoute.h
 * Author: Joshua Johannson
 *
  */
#include <json/JsonObject.h>
#include "json/ApiRoute.h"


ApiRoute::ApiRoute()
{}

ApiRoute::ApiRoute(vector<ApiRoute *> subRoutes)
{
  setSubRoutes(subRoutes);
}

void ApiRoute::progressApi(Json route, string what, Json data)
{
  string component = route.begin().key();
  string entityId  = route.begin().value();

  // pop top route-element
  route.erase(route.begin());
}

void ApiRoute::setSubRoutes(vector<ApiRoute *>& routes)
{
  this->routes = &routes;
}