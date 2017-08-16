//
// Created by joshua on 14.07.17.
//

#include <ltdl.h>
#include "Frontend.h"

using namespace seasocks;

// static
set<WebSocket *> Frontend::webSockConnections;
int Frontend::port;
Server Frontend::server(make_shared<PrintfLogger>
                            (Logger::Level::INFO)
);
function<void (Json data)> Frontend::onData;



/*
 * -- Communication
 */
void Frontend::respond(string status, Json originalMsg)
{
  if (originalMsg.find("respondId") == originalMsg.end())
    return;

  sendData(
      {
          {"type", "respond"},
          {"what", status},
          {"respondId", originalMsg["respondId"]}
      }
  );
}

void Frontend::send(string type, Json what)
{
  sendData(
      {
          {"type", type},
          {"what", what},
      }
  );
}


/* --------------------------------------------------------------------------
 * -- Chart
 */

Frontend::Chart::Chart(string name)
  :tmpGraphs()
{
  this->name = name;
}



Frontend::Chart & Frontend::Chart::setGraphData(string graph,
                                                initializer_list<float> xVals,
                                                initializer_list<float> yVals)
{
  vector<float> x = xVals;
  vector<float> y = yVals;

  return setGraphData(graph, x, y);
}

Frontend::Chart & Frontend::Chart::setGraphData(string graph, vector<float> xVals, vector<float> yVals)
{
  Json data = {
      {"graph", graph},
      {"data", {}}
  };


  for (int i = 0; i < xVals.size(); ++i)
  {
    data["data"].push_back(
        {
            { "x", xVals[i] },
            { "y", yVals[i] }
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

Frontend::Chart & Frontend::getChart(string name)
{
  return *(new Chart(name));
}





// -- start
void Frontend::start(int port)
{
  Frontend::port = port;

  // start server thread
  thread serverThread(serverThreadFunction);
  serverThread.detach();
}




void Frontend::serverThreadFunction()
{
  server.addWebSocketHandler("/", make_shared<WebSocketHandler>());
  server.serve("", port);
}


/* -- WEBSOCKET -------------------------------------*/
void Frontend::WebSocketHandler::onConnect(WebSocket *socket)
{
  webSockConnections.insert(socket);

  cout << "NEW" << endl;
}


void Frontend::sendData(Json data)
{
  server.execute([=](){
    for (auto s : webSockConnections)
      s->send(data.dump());
  });

  // cout << "SEND: " << data.dump() << endl;
}

void Frontend::WebSocketHandler::onData(WebSocket *sock, const char *data)
{
  auto j = Json::parse(data);
  cout << j.dump(2) << endl;
  //sock->send(data);
  if (Frontend::onData) {
    Frontend::onData(j);
    //thread ex(Frontend::onData, j);
    //ex.detach();
  }
}

void Frontend::WebSocketHandler::onDisconnect(WebSocket *socket)
{
  webSockConnections.erase(socket);
}
