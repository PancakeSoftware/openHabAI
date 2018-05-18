//
// Created by joshua on 14.07.17.
//

#include <arpa/inet.h>
#include "Catflow.h"

using namespace seasocks;

// static
ApiProcessible* Catflow::apiRoot = nullptr;

Log Catflow::l("FRONTEND");
set<WebSocket *> Catflow::webSockConnections;
set<set<Client>*> Catflow::linkedWebSockConnections;

int Catflow::port;
int Catflow::portHtml;
list<pair<Client, ApiRequest>> Catflow::requestsToSend;
list<pair<Client, ApiRespond>> Catflow::responsesToSend;


Server Catflow::serverWebsocket(make_shared<SeasocksLogger>( "ServerWebsocket", Logger::Level::INFO));
Server Catflow::serverHttp(make_shared<SeasocksLogger>("ServerHttp", Logger::Level::INFO));

/*
 * -- Communication
 */

void Catflow::send(ApiRequest message)
{
  if (message.client.websocket == nullptr)
    sendData(message.toJson());
  else
    send(message, message.client.websocket);
}

void Catflow::send(ApiRespond message)
{
  if (!message.requestValid || message.request.client.websocket == nullptr)
    sendData(message.toJson());
  else
    send(message, message.request.client.websocket);
}

void Catflow::send(ApiRespond message, Client destination)
{
  // push to queue and store iterator
  responsesToSend.push_back(make_pair(destination, message));
  auto const messageIterator = prev(responsesToSend.end());

  serverWebsocket.execute([=]() mutable {
    //l.info("send ApiRespond to " + string(inet_ntoa(destination->getRemoteAddress().sin_addr)) + ":" + to_string(destination->getRemoteAddress().sin_port));
    destination.websocket->send(message.toJson().dump());
    responsesToSend.erase(messageIterator);
  });
}

void Catflow::send(ApiRequest message, Client destination)
{
    // push to queue and store iterator
  requestsToSend.push_back(make_pair(destination, message));
  auto const messageIterator = prev(requestsToSend.end());

  serverWebsocket.execute([=]() mutable {
    //l.info("send ApiRequest to " + string(inet_ntoa(destination->getRemoteAddress().sin_addr)) + ":" + to_string(destination->getRemoteAddress().sin_port));
    destination.websocket->send(message.toJson().dump());
    requestsToSend.erase(messageIterator);
  });
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




// -- start
void Catflow::start(int portWebsocket, int portHtml)
{
  if (Catflow::apiRoot == nullptr)
    l.warn("you have to set apiRoute!");

  Catflow::portHtml = portHtml;
  Catflow::port = portWebsocket;

  // start server threads
  thread serverWebsocketThread(serverWebsocketThreadFunction);
  serverWebsocketThread.detach();
  l.ok("created serverWebsocket thread");

  thread serverHttpThread(serverHttpThreadFunction);
  serverHttpThread.detach();
  l.ok("created serverHttp thread");
}


void Catflow::serverWebsocketThreadFunction()
{
  serverWebsocket.addWebSocketHandler("/", make_shared<WebSocketHandler>());
  l.ok("listen for webSocket connection on port " + to_string(port));
  serverWebsocket.serve("", port);
  l.info("stop listening to webSocket on port " + to_string(port) + "  [stopped serverWebsocketThread]");
}

void Catflow::serverHttpThreadFunction()
{
  l.info("listen for http connection on port " + to_string(portHtml));
  serverHttp.serve("../../frontend-angular/dist", portHtml);
  l.info("stop listening to http on port " + to_string(portHtml) + "  [stopped serverHttpThread]");
}

/* -- WEBSOCKET -------------------------------------*/
void Catflow::WebSocketHandler::onConnect(WebSocket *socket)
{
  webSockConnections.insert(socket);
  l.info("new connection from '" + socket->getRequestUri() + "'");
}


void Catflow::sendData(Json data)
{
  serverWebsocket.execute([=]() {
    for (auto s : webSockConnections)
      s->send(data.dump());
  });
}



void Catflow::WebSocketHandler::onData(WebSocket *sock, const char *data)
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
      request.client.websocket = sock;
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

void Catflow::WebSocketHandler::onDisconnect(WebSocket *socket)
{
  webSockConnections.erase(socket);

  // remove socket form each linkedWebSockConnections set
  for (auto el: linkedWebSockConnections)
      for(auto removeCandidate = el->begin(); removeCandidate != el->end();) {
          if (removeCandidate->websocket == socket)
              removeCandidate = el->erase(removeCandidate);
          else
              removeCandidate++;
      }
}

void Catflow::registerClientList(set<Client> &list) {
  linkedWebSockConnections.insert(&list);
}
void Catflow::unRegisterClientList(set<Client> &list) {
  linkedWebSockConnections.erase(&list);
}
