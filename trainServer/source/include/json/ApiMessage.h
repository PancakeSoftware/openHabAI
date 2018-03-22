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

class ApiMessage
{
  public:
    virtual Json toJson() { return Json(); };
};

/**
 * @param route route list, defines something similar to rest url
 * @param what  defines action to perform, allowed values depend on route
 * @param data  contains data that is necessary to perform action
 */
class ApiRequest: public ApiMessage
{
  public:
    Json route;
    string what;
    Json data;
    int  respondId = -1;

    ApiRequest(){}
    ApiRequest(Json route, string what)
    {
      this->route = route;
      this->what = what;
    }
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
    }

    virtual Json toJson() override
    {
      return Json {
          {"type", "request"},
          {"what", what},
          {"data", data},
          {"respondId", respondId}
      };
    }
};

class ApiRespond: public ApiMessage
{
  public:
    string what;
    Json data;
    int respondId = -1;
    ApiRequest *request = nullptr;

    ApiRespond() {}
    ApiRespond(ApiRequest request)
    {
      this->request = &request;
      this->respondId = request.respondId;
    }

    virtual Json toJson() override
    {
      return Json {
          {"type", "respond"},
          {"what", what},
          {"data", data},
          {"respondId", respondId}
      };
    }
};

class ApiRespondError : public ApiRespond
{
  public:
    ApiRespondError(string error)
    {
      this->data = Json{{"message", error}};
      this->what = "error";
    }

    ApiRespondError(string error, ApiRequest request) : ApiRespond(request)
    {
      this->data = Json{{"message", error},{"request", request.toJson()}};
      this->what = "error";
    }

    ApiRespondError(string error, Json request)
    {
      this->data = Json{{"message", error},{"request", request}};
      this->what = "error";
    }
};

class ApiRespondOk : public ApiRespond
{
  public:
    ApiRespondOk(Json data, ApiRequest request) : ApiRespond(request)
    {
      this->data = data;
      this->what = "ok";
    }
};

#endif //OPENHABAI_APIMESSAGE_H
