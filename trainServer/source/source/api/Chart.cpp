/* 
 * File:   Chart.h
 * Author: Joshua Johannson
 *
 */
#include <api/ApiMessage.h>
#include <util/TaskManager.h>
#include <Frontend.h>
#include "api/Chart.h"


Chart::Chart()
{
  // subscribers always has to be subset of connections
  Frontend::registerWebsocketList(subscribers);
}

ApiRespond *Chart::processApi(ApiRequest request)
{
  ApiRespond *respond = ApiJsonObject::processApi(request);

  // subscribe
  if (request.what == "subscribe") {
    subscribers.insert(request.websocket);
    info("new subscriber " + to_string(request.websocket));
  }
  if (request.what == "unsubscribe")
    subscribers.erase(request.websocket);

  return respond;
}

void Chart::pushUpdate()
{
  for (auto sub = subscribers.begin(); sub != subscribers.end(); sub++) {
    Frontend::send(ApiRequest(storePath.get(), "update", Json{}), *sub);
  }
}


/**
 * -- ParameterChart -------------------------------------
 */
ParameterChart::ParameterChart()
{
}
ApiRespond *ParameterChart::processApi(ApiRequest request)
{
  ApiRespond *respond = Chart::processApi(request);
  // when chart params change
  if (request.what == "update") {

  }
  return respond;
}
