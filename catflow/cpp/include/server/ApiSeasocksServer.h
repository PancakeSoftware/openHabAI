/* 
 * File:   ApiSeasocksServer
 */
#ifndef OPENHABAI_APISEASOCKSSERVER_H
#define OPENHABAI_APISEASOCKSSERVER_H

#include <Catflow.h>
#include "ApiServer.h"

#ifdef USE_SEASOCKS
#include <seasocks/Server.h>
#include <seasocks/PrintfLogger.h>


/**
 * identifier for client who send a request
 */
class SeasocksClient: public Client {
  public:
    WebSocket* websocket = nullptr;

    SeasocksClient(WebSocket* webSocket) {
      this->websocket = webSocket;
    }

    WebSocket* getWebsocket() const {
      return websocket;
    }

    string toString() {
      if (websocket != nullptr)
        return string(inet_ntoa(websocket->getRemoteAddress().sin_addr)) + ":" + to_string(websocket->getRemoteAddress().sin_port);
      else
        return "??";
    }

    bool operator<(const Client &other) const {
      const SeasocksClient *oth = dynamic_cast<const SeasocksClient*>(&other);
      if (oth != nullptr)
        return websocket < oth->websocket;
      return false;
    }
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


class ApiSeasocksServer: public ApiServer
{
  public:
    ApiSeasocksServer()
        : serverWebsocket(make_shared<SeasocksLogger>("SeasocksServer", Logger::Level::INFO)) {
      setLogName("SeasocksServer");
    }

    void start() override
    {
        // start server threads
        thread serverWebsocketThread(serverWebsocketThreadFunction, this);
        serverWebsocketThread.detach();
        ok("created serverWebsocket thread");
    }
    void startBlocking() override
    {
        ok("will start serverWebsocket blocking");
        serverWebsocketThreadFunction(this);
    }
    void send(Json packet, Client &destination, PacketIdentifier &packetIdentifier) override
    {
        SeasocksClient *dest = dynamic_cast<SeasocksClient*>((Client*)&destination);

        serverWebsocket.execute([=]() mutable {
          dest->websocket->send(packet.dump());
          packetIdentifier.sendDone();
        });
    }

    void sendBroadcast(Json packet) override
    {
        serverWebsocket.execute([=]() {
          for (auto s : webSockConnections)
              s.first->send(packet.dump());
        });
    }


    static void serverWebsocketThreadFunction(ApiSeasocksServer *s)
    {
        s->serverWebsocket.addWebSocketHandler("/", make_shared<WebSocketHandler>(s));
        s->ok("listen for webSocket connection on port " + to_string(Catflow::port));
        s->serverWebsocket.serve("", Catflow::port);
        s->info("stop listening to webSocket on port " + to_string(Catflow::port) + "  [stopped serverWebsocketThread]");
    }

  private:
    Server serverWebsocket;
    map<WebSocket *, SeasocksClient *> webSockConnections;

    struct WebSocketHandler : WebSocket::Handler
    {
        ApiSeasocksServer *server;
        WebSocketHandler(ApiSeasocksServer *server) {
            this->server = server;
        }

        void onConnect(WebSocket *socket) override {
            SeasocksClient *client = new SeasocksClient(socket);
          server->webSockConnections.emplace(socket, client);
            Catflow::onClientConnect(*client);
        }

        void onData(WebSocket *webSocket, const char *data) override {
            auto client = server->webSockConnections.find(webSocket);
            Catflow::onData(*client->second, string(data));
        }
        void onDisconnect(WebSocket *socket) override {
            auto client = server->webSockConnections.find(socket);
            Catflow::onClientDisconnect(*client->second);
            server->webSockConnections.erase(client);
            delete client->second;
        }
    };
};

#endif
#endif //OPENHABAI_APISEASOCKSSERVER_H
