/* 
 * File:   JsonList.h
 * Author: Joshua Johannson
 *
 */
#include "api/JsonObject.h"
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

    ApiProcessible *processible = dynamic_cast<ApiProcessible*>(el->second);
    if (!processible)
      return (ApiRespond*) new ApiRespondError("entity '" + entityIdString + "' cant process any api command", request);

    return processible->processApi(request);
  };


  // JsonList has to remove top route itself, not done by parent ApiRoute
  request.route.erase(request.route.begin());

  // we ar not target if there are still route parts left
  if (request.route.size() > 0) {
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
      items.insert({idAutoIncrement, n});
      if (storePath.is_initialized()) {
        RoutePath nPath = (this->storePath.get());
        (nPath.end()-1)->second = to_string(idAutoIncrement); // set entity id
        n->setStorePath(nPath);
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
  if (!storePath.is_initialized())
    return;

  // foreach dir
  try
  {
    fs::directory_iterator directory_iterator(fs::path(this->storePathString));
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
        ifstream item{storePathString + id + "/item.json"};
        stringstream sItem;
        sItem << item.rdbuf();
        //info("file contend: " + sItem.str());
        params = Json::parse(sItem.str());
      } catch (exception& e) {
        err("read item '"+ storePathString + id + "/item.json" +"' : " + string(e.what()));
        directory_iterator++;
        continue;
      }

      /*
       * add item */
      T *n = createItemFunc(params);
      RoutePath nPath = storePath.get();
      (nPath.end()-1)->second = id; // set entity id
      n->setStorePath(nPath);
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
void JsonList<T>::setStorePath(RoutePath path)
{
  info("setStorePath");
  ApiProcessible::setStorePath(path);
  info("myPath: " + storePathString);
  for (auto i: items) {
    RoutePath n = path;
    //if (n.at(n.size()-1))
    info("path size " + to_string(n.size()));
    (n.end()-1)->second = to_string(i.first); // set entity id
    i.second->setStorePath(n);
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
