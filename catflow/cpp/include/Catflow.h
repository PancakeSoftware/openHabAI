/*
 * Catflow.h
 */

#ifndef CATFLOW_H
#define CATFLOW_H

#include <thread>
#include <seasocks/Server.h>
#include <seasocks/PrintfLogger.h>
#include <json.hpp>
#include <util/Log.h>
#include <ApiMessage.h>
#include <queue>
#include "ApiProcessible.h"

using namespace std;
using namespace seasocks;
using Json = nlohmann::json;

class Catflow
{
  public:
    /**
     * start catflow
     * @param portWebsocket listen for incoming api connections at this port
     * @param portHttp host web-App at this port
     */
    static void start(int portWebsocket, int portHttp);

    /**
     * set api root object
     * @note do this before calling start()
     * @param apiRoot
     */
    static void setApiRootRoute(ApiProcessible &apiRoot){
      Catflow::apiRoot = &apiRoot;
    }

    static void sendData(Json data);
    static void send(ApiRespond message);
    static void send(ApiRequest message);
    static void send(ApiRespond message, Client destination);
    static void send(ApiRequest message, Client destination);

    /**
     * @param list always is subset of connected websockets
     */
    static void registerClientList(set<Client> &list);
    static void unRegisterClientList(set<Client> &list);

    /**
     * @deprecated use Chart from Chart.h
     * chart
     */
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

    /* track packages to send, for testing
     * pair<destination for message, message>
     */
    static list<pair<Client, ApiRequest>> requestsToSend;
    static list<pair<Client, ApiRespond>> responsesToSend;


  private:
    static ApiProcessible* apiRoot;

    static Log l;
    struct WebSocketHandler : WebSocket::Handler
    {
        void onConnect(WebSocket *socket) override;
        void onData(WebSocket *, const char *data) override;
        void onDisconnect(WebSocket *socket) override;
    };

    static Server serverWebsocket;
    static Server serverHttp;

    static set<WebSocket *> webSockConnections;
    static set<set<Client>*> linkedWebSockConnections;

    static void serverWebsocketThreadFunction();
    static void serverHttpThreadFunction();
    static int port;
    static int portHtml;


};

/**
 * own logger for seaSocks
 */
class SeasocksLogger : public seasocks::Logger {
  public:
    SeasocksLogger(string name, Level _minLevelToLog = Level::DEBUG)
        : Logger(),
          minLevelToLog(_minLevelToLog)
    {
      l.setLogName(name);
    }

    ~SeasocksLogger() = default;

    virtual void log(Level level, const char* message) {
      if (level >= minLevelToLog)
      {
        if (level == Level::DEBUG)
          l.debug(message);
        if (level == Level::INFO)
          l.info(message);
        if (level == Level::ACCESS)
          l.info("ACCESS: " + string(message));
        if (level == Level::WARNING)
          l.warn(message);
        if (level == Level::ERROR)
          l.err(message);
        if (level == Level::SEVERE)
          l.info("SEVERE: " + string(message));
      }
    }

    Level minLevelToLog;
    Log l;
};


#endif //CATFLOW_H
