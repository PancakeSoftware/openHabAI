//
// Created by joshua on 14.07.17.
//

#ifndef Frontend_H
#define Frontend_H

#include <thread>
#include <seasocks/Server.h>
#include <seasocks/PrintfLogger.h>
#include <json.hpp>
#include <util/Log.h>
#include <api/ApiMessage.h>

using namespace std;
using namespace seasocks;
using Json = nlohmann::json;

class Frontend
{
  public:
    static void start(int port);

    static void sendData(Json data);
    static void send(ApiRespond message);
    static void send(ApiRequest message);
    static void send(ApiRespond message, WebSocket *destination);
    static void send(ApiRequest message, WebSocket *destination);

    /**
     * @param list always is subset of connected websockets
     */
    static void registerWebsocketList(set<WebSocket*> &list);
    static void unRegisterWebsocketList(set<WebSocket*> &list);

    // chart
    class Chart
    {
      public:
        Chart(string name);
        Chart &setGraphData(string graph, vector<float> xVals, vector<float> yVals);
        Chart &setGraphData(string graph, initializer_list<float> xVals, initializer_list<float> yVals);
        void changeApply(); // send and update chart
        void addApply();    // send and add to chart

      private:
        void apply(string type);
        Json tmpGraphs;
        string name;
    };


    static Chart &getChart(string name);

  private:
    static Log l;
    struct WebSocketHandler : WebSocket::Handler
    {
        void onConnect(WebSocket *socket) override;
        void onData(WebSocket *, const char *data) override;
        void onDisconnect(WebSocket *socket) override;
    };

    static Server server;
    static set<WebSocket *> webSockConnections;
    static set<set<WebSocket *>*> linkedWebSockConnections;
    static void serverThreadFunction();
    static int port;
};


#endif //Frontend_H
