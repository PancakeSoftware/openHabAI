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

/**
 * parses route
 *  /courses/0/students/1/ becomes to vector [courses, 0, students, 1]
 */
class ApiMessageRoute {
  public:
    string pathPrefix; // for storing objects, has to end with /
    vector<string> route;
    vector<string> routeAbsolute; // not effected by pop and push

    ApiMessageRoute() {
    }

    ApiMessageRoute(string routeUrl) {
      fromString(routeUrl);
    }

    void fromString(string routeUrl) {
      route.clear();
      istringstream ss(routeUrl);
      string token;
      while (std::getline(ss, token, '/')) {
        if (token.length() == 0)
          continue;
        route.push_back(token);
      }
      routeAbsolute = route;
    }

    string toString() const {
      string out = "/";
      for (string token: route)
        out += token + "/";
      return out;
    }

    string toStringAbsolute() const {
      string out = "/";
      for (string token: routeAbsolute)
        out += token + "/";
      return out;
    }

    /**
     * @return toSting with pathPrefix before
     */
    string toStringStorePath() const {
      if (pathPrefix.size() == 0)
        return "." + toString();
      else {
        if (pathPrefix.back() == '/')
          return pathPrefix.substr(0, pathPrefix.size()-1)  + toString();
        else
          return pathPrefix  + toString();
      }
    }

    /**
     * pushes element to end route
     * @param token not allowed to contain '/'
     */
    void push(string token) {
      route.push_back(token);
    }

    /**
     * pops first element of route
     * @return
     */
    string pop() {
      if (route.empty())
        return "";
      string token(route.front().c_str());
      route.erase(route.begin());
      return token;
    }

    bool isEmpty() {
      return route.empty();
    }
};

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
    ApiMessageRoute route;
    string what;
    Json data;
    int  respondId = -1;
    WebSocket *websocket = nullptr;

    ApiRequest(){}
    ApiRequest(ApiMessageRoute route, string what)
    {
      this->route = route;
      this->what = what;
    }
    ApiRequest(ApiMessageRoute route, string what, Json data)
    {
      this->route = route;
      this->what = what;
      this->data = data;
    }
    ApiRequest(ApiMessageRoute route, string what, Json data, int respondId)
    {
      this->route = route;
      this->what = what;
      this->data = data;
      this->respondId = respondId;
    }

    ApiRequest(string route, string what)
        : ApiRequest(ApiMessageRoute(route), what) {}
    ApiRequest(string route, string what, Json data)
        : ApiRequest(ApiMessageRoute(route), what, data) {}
    ApiRequest(string route, string what, Json data, int respondId)
        : ApiRequest(ApiMessageRoute(route), what, data, respondId) {}


    virtual Json toJson() override
    {
      if (respondId >= 0)
        return Json {
            {"type", "request"},
            {"route", route.toStringAbsolute()},
            {"what", what},
            {"data", data},
            {"respondId", respondId}
        };
      else
        return Json {
            {"type", "request"},
            {"route", route.toStringAbsolute()},
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

    ApiRespondError(string error, ApiMessageRoute route) : ApiRespond(request)
    {
      this->data.update(Json{{"route", route.toString()}});
    }

    ApiRespondError(string error, ApiRequest request) : ApiRespond(request)
    {
      if (request.respondId < 0)
        this->data = Json{{"message", error},{"request", request.toJson()}};
      else
        this->data = Json{{"message", error}};
      this->what = "error";
    }

    ApiRespondError(string error, ApiRequest request, ApiMessageRoute route) : ApiRespondError(error, request)
    {
      this->data.update(Json{{"route", route.toString()}});
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

    ApiRespondOk(ApiRequest request) : ApiRespond(request)
    {
      this->data = nullptr;
      this->what = "ok";
    }

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
