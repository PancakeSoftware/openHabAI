/*
 * Catflow.h
 */

#ifndef CATFLOW_H
#define CATFLOW_H

#include <thread>
#include <json.hpp>
#include <util/Log.h>
#include <ApiMessage.h>
#include <queue>
#include "ApiProcessible.h"
#include <server/ApiServer.h>
#include <set>

using namespace std;
using Json = nlohmann::json;

class Catflow
{
  public:
    /**
     * start catflow
     * @param port listen for incoming api connections at this port
     * @param httpServerPath path to http root dir to serve
     */
    template <class SERVER>
    static void start(int port, string httpServerPath = "") {
      if (Catflow::apiRoot == nullptr)
        l.warn("you have to set apiRoute!");

      Catflow::port = port;
      Catflow::httpServerPath = httpServerPath;

      // create server
      server = new SERVER();

      // connect
      server->start();
    }

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
    static void send(ApiRespond message, Client &destination);
    static void send(ApiRequest message, Client &destination);

    static void onClientConnect(Client &client);
    static void onClientDisconnect(Client &client);
    static void onData(Client &sender, string data);

    /**
     * @param list always is subset of connected websockets
     */
    static void registerClientList(set<Client*> &list);
    static void unRegisterClientList(set<Client*> &list);








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
    static list<pair<Client*, ApiRequest>> requestsToSend;
    static list<pair<Client*, ApiRespond>> responsesToSend;
    static int port;
    static string httpServerPath;


  private:
    static ApiProcessible* apiRoot;
    static ApiServer* server;

    static Log l;

    static set<Client *> webSockConnections;
    static set<set<Client*>*> linkedWebSockConnections;
};





struct PacketIdentifier {
    virtual void sendDone() {};
};
template <typename T>
struct PacketIdentifierT: public PacketIdentifier {
    T packId;

    PacketIdentifierT(T packId) {
      this->packId = packId;
    }
    virtual void sendDone() {}
};

template <>
struct PacketIdentifierT<list<pair<Client*, ApiRespond>>::const_iterator>: public PacketIdentifier
{
    list<pair<Client*, ApiRespond>>::const_iterator packId;
    PacketIdentifierT(list<pair<Client*, ApiRespond>>::const_iterator packId) {
      this->packId = packId;
    }

    void sendDone() {
      Catflow::responsesToSend.erase(packId);
    }
};

template <>
struct PacketIdentifierT<list<pair<Client*, ApiRequest>>::const_iterator>: public PacketIdentifier  {
    list<pair<Client*, ApiRequest>>::const_iterator packId;
    PacketIdentifierT(list<pair<Client*, ApiRequest>>::const_iterator packId) {
      this->packId = packId;
    }

    void sendDone() {
      Catflow::requestsToSend.erase(packId);
    }
};

#endif //CATFLOW_H
