//
// Created by joshua on 14.07.17.
//

#include <arpa/inet.h>
#include "Catflow.h"
#include <server/ApiServer.h>


// static
ApiProcessible* Catflow::apiRoot = nullptr;
ApiServer* Catflow::server = nullptr;

Log Catflow::l("FRONTEND");
set<Client *> Catflow::webSockConnections;
set<set<Client*>*> Catflow::linkedWebSockConnections;

int Catflow::port;
list<pair<Client*, ApiRequest>> Catflow::requestsToSend;
list<pair<Client*, ApiRespond>> Catflow::responsesToSend;


/*
 * -- Communication
 */
void Catflow::sendData(Json data)
{
  server->sendBroadcast(data);
}

void Catflow::send(ApiRequest message)
{
  if (message.client == nullptr)
    sendData(message.toJson());
  else
    send(message, *message.client);
}

void Catflow::send(ApiRespond message)
{
  if (!message.requestValid || message.request.client == nullptr)
    sendData(message.toJson());
  else
    send(message, *message.request.client);
}

void Catflow::send(ApiRespond message, Client &destination)
{
  // push to queue and store iterator
  responsesToSend.push_back(make_pair(&destination, message));
  auto const messageIterator = prev(responsesToSend.end());
  auto packId = PacketIdentifierT<list<pair<Client*, ApiRespond>>::const_iterator>(messageIterator);
  if (server != nullptr)
    server->send(message.toJson(), destination, packId);
  else
    l.err("cant't send ApiRespond because sever is not NULL, please call Catflow::start() first!");
}

void Catflow::send(ApiRequest message, Client &destination)
{
    // push to queue and store iterator
  requestsToSend.push_back(make_pair(&destination, message));
  auto const messageIterator = prev(requestsToSend.end());
  auto packId = PacketIdentifierT<list<pair<Client*, ApiRequest>>::const_iterator>(messageIterator);
  if (server != nullptr)
    server->send(message.toJson(), destination, packId);
  else
    l.err("cant't send ApiRespond because sever is not NULL, please call Catflow::start() first!");
}

/* --------------------------------------------------------------------------
 * -- Chart
 */

Catflow::Chart::Chart(string name)
    : tmpGraphs()
{
  this->name = name;
}


Catflow::Chart &Catflow::Chart::setGraphData(string graph,
                                               initializer_list<float> xVals,
                                               initializer_list<float> yVals)
{
  vector<float> x = xVals;
  vector<float> y = yVals;

  return setGraphData(graph, x, y);
}

Catflow::Chart &Catflow::Chart::setGraphData(string graph, vector<float> xVals, vector<float> yVals)
{
  Json data = {
      {"graph", graph},
      {"data", {}}
  };


  for (int i = 0; i < xVals.size(); ++i)
  {
    data["data"].push_back(
        {
            {"x", xVals[i]},
            {"y", yVals[i]}
        }
    );
  }


  tmpGraphs.push_back(data);
  return *this;
}

void Catflow::Chart::apply(string type)
{
  Json data = {
      {"type", "updateChart"},
      {"what", "ok"},
      {"data",
       {
           {"chart", this->name},
           {"type", type},
           {"graphs", this->tmpGraphs}
       }
      }
  };

  sendData(data);
}


void Catflow::Chart::addApply()
{
  apply("add");
}

void Catflow::Chart::changeApply()
{
  apply("change");
}

Catflow::Chart &Catflow::getChart(string name)
{
  return *new Chart(name);
}



/* -- SOCKET COMMUNICATION -------------------------------------*/






void Catflow::onData(Client &sender, string data)
{
  //l.debug("got" + string(data));
  if (Catflow::apiRoot == nullptr) {
    l.err("can't progress ApiRequest because apiRoute is not set");
    return;
  }

  /*
   * parse Json */
  Json j;
  try {
    j = Json::parse(data);
  } catch (exception &e) {
    l.err("can't progress api request: can't parse Json ("+  string(e.what()) +"): '"+ string(data) +"'");
  }

  // check if msg has necessary fields
  if (j["type"] != "" and j["route"] != "" and j["what"] != "")
  {
    try
    {
      ApiRequest request(ApiMessageRoute(j["route"].get<string>()), j["what"]);
      request.client = &sender;
      if (j.find("data") != j.end())
        request.data = j["data"];
      if (j.find("respondId") != j.end())
        request.respondId = j["respondId"];

      ApiRespond* response = Catflow::apiRoot->processApi(request);
      if (response != nullptr) {
        Catflow::send(*response);
        delete response;
      }
    } catch (exception &ex) {
      // error
      Catflow::send(ApiRespondError("can't progress api request ("+ string(ex.what()) +")", j));
      l.err("can't progress api request ("+  string(ex.what()) +"): '"+ j.dump(2) +"'");
    }
  }
}


// -- mange clients
void Catflow::onClientConnect(Client &client)
{
  webSockConnections.insert(&client);
  l.info("new connection from '" + client.toString() + "'");
}
void Catflow::onClientDisconnect(Client &client)
{
  webSockConnections.erase(&client);
  l.info("connection gone '" + client.toString() + "'");

  // remove socket form each linkedWebSockConnections set
  for (auto el: linkedWebSockConnections)
    for(auto removeCandidate = el->begin(); removeCandidate != el->end();) {
      if (*removeCandidate == &client)
        removeCandidate = el->erase(removeCandidate);
      else
        removeCandidate++;
    }
}

void Catflow::registerClientList(set<Client*> &list) {
  linkedWebSockConnections.insert(&list);
}
void Catflow::unRegisterClientList(set<Client*> &list) {
  linkedWebSockConnections.erase(&list);
}