//
// Created by joshua on 14.07.17.
//

#ifndef Frontend_H
#define Frontend_H

#include <thread>
#include <seasocks/Server.h>
#include <seasocks/PrintfLogger.h>
#include <json.hpp>

using namespace std;
using namespace seasocks;
using Json = nlohmann::json;

class Frontend
{
  public:
    static void start(int port);

    static void sendData(Json data);
    static void send(string type, Json what);
    static function<void (Json data)> onData;

    static void respond(string status, Json originalMsg);


    // chart
    class Chart {
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

    struct WebSocketHandler : WebSocket::Handler {
        void onConnect(WebSocket *socket) override;
        void onData(WebSocket *, const char *data) override;
        void onDisconnect(WebSocket *socket) override;
    };

    static Server server;
    static set<WebSocket *> webSockConnections;
    static void serverThreadFunction();
    static int port;
};


#endif //Frontend_H
