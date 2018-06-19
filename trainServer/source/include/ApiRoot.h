/* 
 * File:   ApiRoot.h
 * Author: Joshua Johannson
 *
 */
#ifndef APIROOT_H
#define APIROOT_H

#include <json.hpp>
#include <ApiRoute.h>
#include <dataStructures/DataStructure.h>
#include "Catflow.h"
#include "Settings.h"

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
    JsonList<DataStructure> datastructures{};
    Settings settings{};

    ApiRoot()
    {
      setSubRoutes({
                       {"dataStructures", &datastructures},
                       {"settings", &settings}
      });
      datastructures.setCreateItemFunction(DataStructure::create);
    }
};

#endif //APIROOT_H
