/* 
 * File:   ApiRoot.h
 * Author: Joshua Johannson
 *
 */
#ifndef APIROOT_H
#define APIROOT_H

#include <json.hpp>
#include <api/ApiRoute.h>
#include <dataStructures/DataStructure.h>
#include "Frontend.h"

using namespace std;
using Json = nlohmann::json;


class DataStructure;
/*
 * ApiRoot class
 * holds dataStructures List
 */
class ApiRoot: public ApiRoute
{
  public:
    JsonList<DataStructure> datastructures;

    ApiRoot() :ApiRoute({{"dataStructures", &datastructures}})
    {
      datastructures.setCreateItemFunction(DataStructure::create);
    }
};

extern ApiRoot apiRoot;

#endif //APIROOT_H
