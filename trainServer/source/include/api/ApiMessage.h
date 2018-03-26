/* 
 * File:   ApiMessage.h
 * Author: Joshua Johannson
 *
  */
#ifndef OPENHABAI_APIMESSAGE_H
#define OPENHABAI_APIMESSAGE_H

#include <json.hpp>
#include <string>
#include <seasocks/WebSocket.h>
#include <arpa/inet.h>
using Json = nlohmann::json;
using namespace std;
using namespace seasocks;
typedef vector<pair<string, string>> RoutePath;

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
    WebSocket *websocket = nullptr;

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
    ApiRequest(RoutePath route, string what){
      Json j{};
      for (auto item : route)
        j.push_back({{item.first, item.second}});
      this->route = j;
      this->what = what;
    }
    ApiRequest(RoutePath route, string what, Json data) : ApiRequest(route, what) {
      this->data = data;
    }


    virtual Json toJson() override
    {
      if (respondId >= 0)
        return Json {
            {"type", "request"},
            {"route", route},
            {"what", what},
            {"data", data},
            {"respondId", respondId}
        };
      else
        return Json {
            {"type", "request"},
            {"route", route},
            {"what", what},
            {"data", data}
        };
    }
};

class ApiRespond: public ApiMessage
{
  public:
    string what;
    Json data;
    int respondId = -1;

    ApiRequest request;
    bool requestValid = false;

    ApiRespond() {}
    ApiRespond(ApiRequest request)
    {
      requestValid = true;
      this->request = request;
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

    ~ApiRespond() {
      //if (request != nullptr)
      //  delete request;
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

inline string to_string(WebSocket *socket) {
  return string(inet_ntoa(socket->getRemoteAddress().sin_addr)) + ":" + to_string(socket->getRemoteAddress().sin_port);
}

#endif //OPENHABAI_APIMESSAGE_H
