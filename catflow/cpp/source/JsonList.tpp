/* 
 * File:   JsonList.h
 * Author: Joshua Johannson
 *
 */
#include "ApiJsonObject.h"
#include "JsonList.h"
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

template<class T>
JsonList<T>::JsonList(): JsonList(defaultIdAutoIncrement)
{}
template<class T>
JsonList<T>::JsonList(int &idAutoIncrement)
    : ApiJsonObject(),
      idAutoIncrement(idAutoIncrement)
{
  setLogName("JsonList");
}
template<class T>
ApiRespond* JsonList<T>::processApi(ApiRequest request)
{
  ApiRespond *respond = nullptr;

  // action to one entity
  bool destEntity = !request.route.isEmpty();

  // entity id
  string entityIdString = request.route.pop();
  int entityId = -1;

  //info("at " + component + "/" + entityIdString);

  // if for list item
  if (destEntity)
  {
    // to int
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
  else // for me
  {
    // processed by ApiJsonObject part
    respond = ApiJsonObject::processApi(request);
    if (respond != nullptr)
      return respond;
  }


  // call process api of subRoute
  function<ApiRespond*()> callProcessApiOfItem = [&](){
    auto el = items.find(entityId);
    if (el == items.end())
      return (ApiRespond*) new ApiRespondError("entity '" + entityIdString + "'  not found", request, route.get_value_or(ApiMessageRoute()));

    ApiProcessible *processible = dynamic_cast<ApiProcessible*>(el->second);
    if (!processible)
      return (ApiRespond*) new ApiRespondError("entity '" + entityIdString + "' cant process any api command because"
                                                   + ((is_fundamental<T>::value) ? "it is a primitive type " : "") + "(Type: "+ typeid(T).name()+")", request);

    return processible->processApi(request);
  };

  // test constexpr
  //if constexpr (is_fundamental<T>::value) {
  //  info("progressApi of fundamental T type: " + string(typeid(T).name()));
  //}

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
      info("at "+route.get_value_or(ApiMessageRoute()).toString()+" add " + to_string(idAutoIncrement));

      Json j = request.data;
      j.update(Json{{"id", idAutoIncrement}}); // insert id key into data json
      T* n = createItemFunc(j);
      if (n != nullptr)
      {
        items.insert({idAutoIncrement, n});
        if (route.is_initialized() && is_base_of<ApiJsonObject, T>::value)
        {
          ApiMessageRoute nRoute = this->route.get_value_or(ApiMessageRoute());
          nRoute.push(to_string(idAutoIncrement)); // set entity id in its route
          n->setRoute(nRoute);
          n->store();
        }

        // subscriber update
        ApiMessageRoute removedRoute = route.is_initialized() ? route.get() : ApiMessageRoute("");
        removedRoute.push(to_string(idAutoIncrement));
        sendToSubscribers(ApiRequest(removedRoute, "add", n->toJson()),
                          *request.client);

        idAutoIncrement++;
        return new ApiRespondOk(n->toJson(), request);
      }
      else
        return new ApiRespondError("can’t create item by given data", request, route.get_value_or(ApiMessageRoute()));
    }
  }


  if (destEntity)
  {
    /*
     * Destination: single entity
     */
    auto item = items.find(entityId);
    if (item == items.end())
    {
      return new ApiRespondError("entity '" + entityIdString + "' not found", request, route.get_value_or(ApiMessageRoute()));
    }

    // get item
    if (request.what == "get")
    {
      return new ApiRespondOk((*item).second->toJson(), request);
    }

    // remove item
    if (request.what == "remove")
    {
      if (route.is_initialized())
      {
        info("at " + route.get().toString() + " remove " + to_string(item->first));
        item->second->remove();
        boost::filesystem::remove_all(route.get().toStringStorePath() + entityIdString);
        info("removed item " + route.get().toStringStorePath());
        items.erase(item);
        delete item->second;

        // subscriber update
        ApiMessageRoute removedRoute = route.is_initialized() ? route.get() : ApiMessageRoute("");
        removedRoute.push(entityIdString);
        sendToSubscribers(ApiRequest(removedRoute, "remove"),
                          *request.client);

        return new ApiRespondOk(request);
      }
    }
  }


  // the what commands goal is item
  if (destEntity)
    return callProcessApiOfItem();
  return respond; // is nullptr
}


template<class T>
T &JsonList<T>::get(int id)
{
  return *(*items.find(id)).second;
}

template<class T>
void JsonList<T>::set(int id, T element)
{
  T *n = new T(element);
  n->fromJson(Json{{"id", id}});
  items.insert({id, n});
  if (route.is_initialized() && is_base_of<ApiJsonObject, T>::value)
  {
    ApiMessageRoute nRoute = this->route.get_value_or(ApiMessageRoute());
    nRoute.push(to_string(idAutoIncrement)); // set entity id in its route
    n->setRoute(nRoute);
    n->store();
  }
  idAutoIncrement++;
}

template<class T>
void JsonList<T>::add(T element)
{
  T *n = new T(element);
  n->fromJson(Json{{"id", idAutoIncrement}});
  items.insert({idAutoIncrement, n});
  if (route.is_initialized() && is_base_of<ApiJsonObject, T>::value)
  {
    ApiMessageRoute nRoute = this->route.get_value_or(ApiMessageRoute());
    nRoute.push(to_string(idAutoIncrement)); // set entity id in its route
    n->setRoute(nRoute);
    n->store();
  }

  // subscriber update
  ApiMessageRoute removedRoute = route.is_initialized() ? route.get() : ApiMessageRoute("");
  removedRoute.push(to_string(idAutoIncrement));
  sendToSubscribers(ApiRequest(removedRoute, "add", n->toJson()));

  idAutoIncrement++;
}

template<class T>
bool JsonList<T>::remove(int id)
{
  auto it = items.find(id);
  if (it != items.end()) {
    items.erase(it);

    // subscriber update
    ApiMessageRoute removedRoute = route.is_initialized() ? route.get() : ApiMessageRoute("");
    removedRoute.push(to_string(id));
    sendToSubscribers(ApiRequest(removedRoute, "remove"));

    return true;
  }
  return false;
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
      try
      {
        ifstream item{this->route.get().toStringStorePath() + id + "/item.json"};
        stringstream sItem;
        sItem << item.rdbuf();
        params = Json::parse(sItem.str());
      }
      catch (exception& e) {
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
      if (idNum >= idAutoIncrement)
        idAutoIncrement = idNum + 1;
      directory_iterator++;
    }
  }
  catch (exception &e){
    err("restore ("+this->route.get().toStringStorePath()+"): " + string(e.what()));
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

template<class T>
void JsonList<T>::remove()
{
  ApiProcessible::remove();
  for (auto item : items) {
    item.second->remove();
    delete item.second;
  }
  items.clear();
  boost::filesystem::remove_all(route.get().toStringStorePath());
  info("removed " + route.get().toStringStorePath());
}

template<class T>
JsonList<T>::~JsonList()
{
  info("at "+route.get_value_or(ApiMessageRoute()).toString()+" will delete all items ...");
  for (auto item : items)
    delete item.second;
  items.clear();
}


