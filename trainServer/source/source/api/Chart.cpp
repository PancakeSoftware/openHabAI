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
  addJsonParams({{}
                });
}

ApiRespond *Chart::processApi(ApiRequest request)
{
  ApiRespond *respond = JsonObject::processApi(request);
  // subscribe
  if (request.what == "subscribe") {
    TaskManager::addTaskOnceOnly([this]{
      Frontend::send(ApiRequest(this->storePath.get(), "add"));
    });
  }
  return respond;
}


/**
 * -- ParameterChart -------------------------------------
 */
ParameterChart::ParameterChart()
{
  addJsonParams({{"range_from", &range_from}
                });
}
ApiRespond *ParameterChart::processApi(ApiRequest request)
{
  ApiRespond *respond = JsonObject::processApi(request);
  // when chart params change
  if (request.what == "update") {

  }
  return respond;
}
