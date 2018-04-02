/* 
 * File:   JsonList.h
 * Author: Joshua Johannson
 *
 */
#include "api/ApiJsonObject.h"
#include "api/JsonList.h"
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

template<class T>
JsonList<T>::JsonList(): JsonList(defaultIdAutoIncrement)
{}
template<class T>
JsonList<T>::JsonList(int &idAutoIncrement)
    : idAutoIncrement(idAutoIncrement)
{
  setLogName("JsonList");
}
template<class T>
ApiRespond* JsonList<T>::processApi(ApiRequest request)
{
  // action to one entity
  bool destEntity = !request.route.isEmpty();

  // entity id
  string entityIdString = request.route.pop();
  int entityId = -1;

  //info("at " + component + "/" + entityIdString);

  // to int
  if (destEntity)
  {
    try
    {
      entityId = stoi(entityIdString);
    } catch (invalid_argument e)
    {
      err("entity's id is not a number", e.what());
      destEntity = false;
      return new ApiRespondError("entity's id is not a number '" +string(e.what())+"'", request);
    }
  }


  // call process api of subRoute
  function<ApiRespond*()> callProcessApiOfItem = [&](){
    auto el = items.find(entityId);
    if (el == items.end())
      return (ApiRespond*) new ApiRespondError("entity '" + entityIdString + "' of '"+ route.get_value_or(ApiMessageRoute()).toStringAbsolute() +"' not found", request);

    ApiProcessible *processible = dynamic_cast<ApiProcessible*>(el->second);
    if (!processible)
      return (ApiRespond*) new ApiRespondError("entity '" + entityIdString + "' cant process any api command because"
                                                   + ((is_fundamental<T>::value) ? "it is a primitive type " : "") + "(Type: "+ typeid(T).name()+")", request);

    return processible->processApi(request);
  };


  // we ar not target if there are still route parts left
  if (!request.route.isEmpty()) {
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
      Json j = request.data;
      j.update(Json{{"id", idAutoIncrement}}); // insert id key into data json
      T* n = createItemFunc(j);
      items.insert({idAutoIncrement, n});
      if (route.is_initialized()) {
        ApiMessageRoute nRoute = this->route.get();
        nRoute.push(to_string(idAutoIncrement)); // set entity id in its route
        n->setRoute(nRoute);
        n->store();
      }
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
void JsonList<T>::setCreateItemFunction(function<T *(Json params)> createItemFunc)
{
  this->createItemFunc = createItemFunc;
}

template<class T>
void JsonList<T>::restore()
{
  ApiProcessible::restore();
  if (!route.is_initialized())
    return;

  // foreach dir
  try
  {
    fs::directory_iterator directory_iterator(fs::path(this->route.get().toStringStorePath()));
    while (directory_iterator != fs::directory_iterator{})
    {
      /*
       * get id by directory name */
      string id = directory_iterator->path().filename().string();
      int idNum;
      try
      {
        idNum = stoi(id);
      } catch (invalid_argument& e)
      {
        err("entity's id (in directoryName '" + directory_iterator->path().string() + "') is not a number", e.what());
        continue;
      }
      //info("... restore: " + storePath.get() + "/" + id);

      /*
       * read item.json */
      Json params;
      try {
        ifstream item{this->route.get().toStringStorePath() + id + "/item.json"};
        stringstream sItem;
        sItem << item.rdbuf();
        //info("file contend: " + sItem.str());
        params = Json::parse(sItem.str());
      } catch (exception& e) {
        err("read item '"+ this->route.get().toStringStorePath() + id + "/item.json" +"' : " + string(e.what()));
        directory_iterator++;
        continue;
      }

      /*
       * add item */
      T *n = createItemFunc(params);
      ApiMessageRoute nRoute = route.get();
      nRoute.push(id); // set entity id
      n->setRoute(nRoute);
      items.insert({idNum, n});
      n->restore();

      // next
      idAutoIncrement++;
      directory_iterator++;
    }
  }
  catch (exception &e){
    err("restore: " + string(e.what()));
  }
}

template<class T>
void JsonList<T>::setRoute(ApiMessageRoute route)
{
  info("setStorePath");
  ApiProcessible::setRoute(route);
  info("myPath: " + routeString);
  for (auto i: items) {
    ApiMessageRoute n = route;
    //if (n.at(n.size()-1))
    info("path size " + to_string(n.route.size()));
    n.push(to_string(i.first)); // set entity id
    i.second->setRoute(n);
  }
}

template<class T>
void JsonList<T>::store()
{
  ApiProcessible::store();
  for (auto i: items) {
    i.second->store();
  }
}
