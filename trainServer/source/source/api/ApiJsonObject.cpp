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

vector<string> ApiJsonObject::fromJson(Json params)
{
  vector<string> changedParams = JsonObject::fromJson(params);

  // save all
  storeMe();

  // push updates to subscribers
  if (route.is_initialized())
    sendToSubscribers(ApiRequest(route.get(), "update", toJson(changedParams)));

  return changedParams;
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
  ApiRespond *processible =  ApiSubscribable::processApi(request);

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

  return processible;
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

void ApiJsonObject::remove()
{
  ApiProcessible::remove();
  if (route.is_initialized())
    boost::filesystem::remove(route.get().toStringStorePath() + "item.json");
}

void ApiJsonObject::notifyParamsChanged(string paramC ...) {
  if (!route.is_initialized()) {
      warn("can’t push update to subscribers at notifyParamsChanged(...) because route of Object is not set, maybe you forgot to call setRoute() or setStorePath().");
      return;
  }

  vector<string> params = flatten(paramC);
    warn(accumulate(params.begin(), params.end(), string("notifyParamsChanged(), please use notifyParamsChanged(vector<string>) because this method seems not to work: changed params("+to_string(params.size())+"): "), [](string text, string param) {
        return text + param + ",";
    }));

  // send changed params to subscribers
  sendToSubscribers(
          ApiRequest(route.get(),
                     "update",
                     toJson(params))
  );
}

void ApiJsonObject::notifyParamsChanged(vector<string> params) {
  if (!route.is_initialized()) {
    warn("can’t push update to subscribers at notifyParamsChanged(...) because route of Object is not set, maybe you forgot to call setRoute() or setStorePath().");
    return;
  }

  // send changed params to subscribers
  sendToSubscribers(
          ApiRequest(route.get(),
                     "update",
                     toJson(params))
  );
}

