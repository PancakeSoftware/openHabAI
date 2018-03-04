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
ApiRespond* JsonList<T>::processApi(ApiRequest request)
{

  string component = (*request.route.begin()).begin().key(); // already progressed by parent
  string entityIdString = (*request.route.begin()).begin().value().get<string>();
  int entityId = -1;

  //info("at " + component + "/" + entityIdString);

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


  // call process api of subRoute
  function<ApiRespond*()> callProcessApiOfItem = [&](){
    auto el = items.find(entityId);
    if (el == items.end())
      return (ApiRespond*) new ApiRespondError("entity '" + entityIdString + "' of '"+ component +"' not found", request);

    ApiProcessible *processible = dynamic_cast<ApiProcessible*>((*el).second);
    if (!processible)
      return (ApiRespond*) new ApiRespondError("entity '" + entityIdString + "' cant process any api command", request);

    return processible->processApi(request);
  };


  // JsonList has to remove top route itself, not done by parent ApiRoute
  request.route.begin()->erase(request.route.begin()->begin());

  // we ar not target if there are still route parts left
  if (request.route.begin()->size() > 0) {
    return callProcessApiOfItem();
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

      return new ApiRespondOk(res, request);
    }

    // add
    if (request.what == "add")
    {
      T* n = createItemFunc(request.data.merge(Json {{"id", idAutoIncrement}}));
      items.insert({
                       idAutoIncrement,
                       n
                   });
      idAutoIncrement++;
      return new ApiRespondOk(n->toJson(), request);
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
        return new ApiRespondError("entity '" + entityIdString + "' not found", request);
      }

      return new ApiRespondOk((*el).second->toJson(), request);
    }
  }


  // the what commands goal is item
  return callProcessApiOfItem();
}

template<class T>
bool JsonList<T>::loadAll()
{

}

template<class T>
T &JsonList<T>::get(int id)
{
  return *(*items.find(id)).second;
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