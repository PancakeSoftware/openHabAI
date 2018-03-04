/* 
 * File:   ApiMessage.h
 * Author: Joshua Johannson
 *
  */
#ifndef OPENHABAI_APIMESSAGE_H
#define OPENHABAI_APIMESSAGE_H

#include <json.hpp>
#include <string>
using Json = nlohmann::json;
using namespace std;

/**
 * @param route route list, defines something similar to rest url
 * @param what  defines action to perform, allowed values depend on route
 * @param data  contains data that is necessary to perform action
 */
struct ApiRequest
{
    Json route;
    string what;
    Json data;
    int  respondId = -1;
    bool respondWanted = false;

    ApiRequest(){}
    ApiRequest(Json route, string what, Json data)
    {
      this->route = route;
      this->what = what;
      this->data = data;
    }
    ApiRequest(Json route, string what, Json data, int respondId)
    {
      this->route = route;
      this->what = what;
      this->data = data;
      this->respondId = respondId;
      this->respondWanted = true;
    }
};

struct ApiRespond
{
    string what;
    Json data;
    int resondId = -1;

    ApiRespond(ApiRequest request)
    {
      this->resondId = request.respondId;
    }

    virtual Json toJson()
    {
      return Json {
          {"type", "respond"},
          {"what", what},
          {"data", data},
          {"respondId", resondId}
      };
    }
};

struct ApiRespondError : ApiRespond
{
    ApiRespondError(string error, ApiRequest request) : ApiRespond(request)
    {
      this->data = Json{{"message", error}};
      this->what = "error";
    }
};

struct ApiRespondOk : ApiRespond
{
    ApiRespondOk(Json data, ApiRequest request) : ApiRespond(request)
    {
      this->data = data;
      this->what = "ok";
    }
};

#endif //OPENHABAI_APIMESSAGE_H
