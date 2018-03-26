//
// Created by joshua on 14.07.17.
//

#include <ApiRoot.h>
#include <arpa/inet.h>
#include "Frontend.h"

using namespace seasocks;

// static
Log Frontend::l("FRONTEND");
set<WebSocket *> Frontend::webSockConnections;
set<set<WebSocket *>*> Frontend::linkedWebSockConnections;
int Frontend::port;
Server Frontend::server(make_shared<PrintfLogger>
                            (Logger::Level::INFO)
);


/*
 * -- Communication
 */

void Frontend::send(ApiRequest message)
{
  if (message.websocket == nullptr)
    sendData(message.toJson());
  else
    send(message, message.websocket);
}

void Frontend::send(ApiRespond message)
{
  if (!message.requestValid || message.request.websocket == nullptr)
    sendData(message.toJson());
  else
    send(message, message.request.websocket);
}

void Frontend::send(ApiRespond message, WebSocket *destination)
{
  server.execute([=]() mutable {
    l.info("send ApiRespond to " + string(inet_ntoa(destination->getRemoteAddress().sin_addr)) + ":" + to_string(destination->getRemoteAddress().sin_port));
    destination->send(message.toJson().dump());
  });
}

void Frontend::send(ApiRequest message, WebSocket *destination)
{
  server.execute([=]() mutable {
    l.info("send ApiRequest to " + string(inet_ntoa(destination->getRemoteAddress().sin_addr)) + ":" + to_string(destination->getRemoteAddress().sin_port));
    destination->send(message.toJson().dump());
  });
}

/* --------------------------------------------------------------------------
 * -- Chart
 */

Frontend::Chart::Chart(string name)
    : tmpGraphs()
{
  this->name = name;
}


Frontend::Chart &Frontend::Chart::setGraphData(string graph,
                                               initializer_list<float> xVals,
                                               initializer_list<float> yVals)
{
  vector<float> x = xVals;
  vector<float> y = yVals;

  return setGraphData(graph, x, y);
}

Frontend::Chart &Frontend::Chart::setGraphData(string graph, vector<float> xVals, vector<float> yVals)
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

void Frontend::Chart::apply(string type)
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


void Frontend::Chart::addApply()
{
  apply("add");
}

void Frontend::Chart::changeApply()
{
  apply("change");
}

Frontend::Chart &Frontend::getChart(string name)
{
  return *new Chart(name);
}




// -- start
void Frontend::start(int port)
{
  Frontend::port = port;

  // start server thread
  thread serverThread(serverThreadFunction);
  serverThread.detach();
  l.ok("create socket thread");
}


void Frontend::serverThreadFunction()
{
  server.addWebSocketHandler("/", make_shared<WebSocketHandler>());
  l.ok("listen for connection on port " + to_string(port));
  server.serve("", port);
  l.ok("stop listening to port" + to_string(port));
}


/* -- WEBSOCKET -------------------------------------*/
void Frontend::WebSocketHandler::onConnect(WebSocket *socket)
{
  webSockConnections.insert(socket);
  l.info("new connection from '" + socket->getRequestUri() + "'");
}


void Frontend::sendData(Json data)
{
  server.execute([=]() {
    for (auto s : webSockConnections)
      s->send(data.dump());
  });
}



void Frontend::WebSocketHandler::onData(WebSocket *sock, const char *data)
{
  /*
   * parse Json */
  Json j;
  try {
    j = Json::parse(data);
  } catch (exception &e) {
    l.err("can't progress api request: can't parse Json ("+  string(e.what()) +"): '"+ data +"'");
  }

  // check if msg has necessary fields
  if (j["type"] != "" and j["what"] != "")
  {
    try
    {
      ApiRequest request(j["route"], j["what"]);
      request.websocket = sock;
      if (j.find("data") != j.end())
        request.data = j["data"];
      if (j.find("respondId") != j.end())
        request.respondId = j["respondId"];

      ApiRespond* response = apiRoot.processApi(request);
      if (response != nullptr) {
        Frontend::send(*response);
        delete response;
      }
    } catch (logic_error &ex) {
      // error
      Frontend::send(ApiRespondError("can't progress api request ("+ string(ex.what()) +")", j));
      l.err("can't progress api request ("+  string(ex.what()) +"): '"+ j.dump(2) +"'");
    }
    catch (exception &e) {
      l.err("can't progress api request general error ("+  string(e.what()) +"): '"+ j.dump(2) +"'");
    }
  }
}

void Frontend::WebSocketHandler::onDisconnect(WebSocket *socket)
{
  webSockConnections.erase(socket);
  for (auto el: linkedWebSockConnections)
    el->erase(socket);
}

void Frontend::registerWebsocketList(set<WebSocket *> &list) {
  linkedWebSockConnections.insert(&list);
}
void Frontend::unRegisterWebsocketList(set<WebSocket *> &list) {
  linkedWebSockConnections.erase(&list);
}
