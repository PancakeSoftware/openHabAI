/* 
 * File:   JsonList.h
 * Author: Joshua Johannson
 *
  */
#include <json/JsonObject.h>
#include "json/JsonList.h"

template<class T>
JsonList<T>::JsonList(int &idAutoIncrement, string folder, string entryFileName)
    : idAutoIncrement(idAutoIncrement)
{
  setLogName("JSLI");
}
template<class T>
Json JsonList<T>::progressApi(ApiRequest &request)
{
  string component = (*request.route.begin()).begin().key(); // already progressed by parent
  string entityIdString = (*request.route.begin()).begin().value().get<string>();
  int entityId = -1;

  info("at " + component + "/" + entityIdString);

  // check if destination is entity
  bool destEntity = (entityIdString.length() > 0);
  if (destEntity)
  {
    try
    {
      entityId = stoi(entityIdString);
    } catch (invalid_argument e)
    {
      err("entity's id is not a number", e.what());
      destEntity = false;
    }
  }


  if (!destEntity)
  {
    /*
     * Destination: whole list
     */
    // getAll
    if (request.what == "getAll")
    {
      Json res{};
      for (auto el : items)
        res.push_back(el.second->toJson());

      return Json {
          {"type", "respond"},
          {"what", "ok"},
          {"data", res},
          {"respondId", request.resondId}
      };
    }

    // add
    if (request.what == "add")
    {

      items.insert({
                       idAutoIncrement,
                       createItemFunc(request.data.merge(Json {{"id", idAutoIncrement}}))
                   });
      idAutoIncrement++;
    }
  }


  if (destEntity)
  {
    /*
     * Destination: single entity
     */

    if (request.what == "get")
    {
      auto el = items.find(entityId);
      if (el == items.end())
      {
        return Json {
            {"type", "respond"},
            {"what", "error"},
            {"data",
             {"message", "entity '" + entityIdString + "' not found"}
            },
            {"respondId", request.resondId}
        };
      }

      return Json {
          {"type", "respond"},
          {"what", "ok"},
          {"data", (*el).second->toJson()},
          {"respondId", request.resondId}
      };
    }
  }


  // pop top route-element
  request.route.erase(request.route.begin());
}

template<class T>
bool JsonList<T>::loadAll()
{

}

template<class T>
T &JsonList<T>::get(int id)
{

}

template<class T>
int JsonList<T>::length()
{
  return items.size();
}

template<class T>
void JsonList<T>::progressCommand(Json command)
{

}

template<class T>
void JsonList<T>::setCreateItemFunction(function<T *(Json params)> createItemFunc)
{
  this->createItemFunc = createItemFunc;
}