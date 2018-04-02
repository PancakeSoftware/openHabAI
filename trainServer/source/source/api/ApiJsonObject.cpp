/*
 * File:   JsonObject.h
 * Author: Joshua Johannson
 *
 */
#include "api/ApiJsonObject.h"
#include "api/Chart.h"
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

Log JsonObject::l("JsonObject");


/*
 * == ApiJsonObject ======================================================
 */
ApiJsonObject::ApiJsonObject()
{
  setLogName("ApiJsonObject");
}

ApiJsonObject::ApiJsonObject(Json params)
:ApiJsonObject()
{
  fromJson(params);
}

void ApiJsonObject::fromJson(Json params)
{
  JsonObject::fromJson(params);

  // save all
  storeMe();
}


// -- SAVE LOAD --------------------------------------------------------
bool ApiJsonObject::save(string path, string fileName)
{
  string full = path + fileName;

  try {
    fs::create_directories(path);
  }
  catch(exception &e) {
    err("save to " + full, "can not create directory: " + string(e.what()));
    return false;
  }

  ofstream stream;
  stream.open(full);
  if (!stream.is_open()) {
    stream.close();
    err("save to " + full);
    return false;
  }
  stream << toJson().dump(2);
  stream.close();
  ok("save to " + full);
  return true;
}

bool ApiJsonObject::load(string path, string fileName)
{
  string full = path  + fileName;

  Json params;
  try {
    ifstream file{full};
    stringstream sItem;
    sItem << file.rdbuf();
    //info("file contend: " + sItem.str());
    if (sItem.str().length() == 0)
      throw domain_error("file is empty");
    params = Json::parse(sItem.str()); // @TODO somehow catch is not working for parse_error
  } catch (Json::parse_error &e) {
    err("load from '"+ full +"' can’t parse json", string(e.what()));
    return false;
  } catch (exception &e) {
    err("load from '"+ full +"' ", string(e.what()));
    return false;
  }

  fromJson(params);
  ok("load from " + full);
  return true;
}



ApiRespond *ApiJsonObject::processApi(ApiRequest request)
{
  if (request.what == "update")
  {
    try {
      this->fromJson(request.data);
    } catch (JsonObjectException &e) {
      return new ApiRespondError(e.what(), request, route.get());
    }
  }
  if (request.what == "get")
    return new ApiRespondOk(this->toJson(), request);
  return nullptr;
}

void ApiJsonObject::restore()
{
  ApiProcessible::restore();
  if (route.is_initialized())
    load(route.get().toStringStorePath(), "item.json");
}

void ApiJsonObject::setRoute(ApiMessageRoute route)
{
  ApiProcessible::setRoute(route);
}
void ApiJsonObject::store()
{
  ApiProcessible::store();
  storeMe();
}
void ApiJsonObject::storeMe()
{
  ApiProcessible::storeMe();
  if (route.is_initialized())
    save(route.get().toStringStorePath(), "item.json");
}
