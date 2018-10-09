/* 
 * File:   ApiSeasocksServer
 */
#ifndef OPENHABAI_APISUWEBSOCKETSSERVER_H
#define OPENHABAI_APISUWEBSOCKETSSERVER_H

#include "../Catflow.h"
#include "ApiServer.h"
#include <uWS.h>
#include <thread>
#include <boost/filesystem.hpp>


/**
 * identifier for client who send a request
 */
class UWebsocketsClient: public Client {
  public:
    uWS::WebSocket<uWS::SERVER>* websocket = nullptr;

    UWebsocketsClient(uWS::WebSocket<uWS::SERVER>* webSocket) {
      this->websocket = webSocket;
    }

    uWS::WebSocket<uWS::SERVER>* getWebsocket() const {
      return websocket;
    }

    string toString() {
      if (websocket != nullptr)
        return string(websocket->getAddress().address) + " :" + to_string(websocket->getAddress().port);
      else
        return "??";
    }

    bool operator<(const Client &other) const {
      const UWebsocketsClient *oth = dynamic_cast<const UWebsocketsClient*>(&other);
      if (oth != nullptr)
        return websocket < oth->websocket;
      return false;
    }
};



class ApiuWebsocketsServer: public ApiServer
{
  public:
    ApiuWebsocketsServer()
    {
      setLogName("uWebsocketServer");

      // set callbacks
      hub.onMessage([this](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
        auto client = webSockConnections.find(ws);
        Catflow::onData(*client->second, string(message).substr(0, length));
      });

      hub.onConnection([this](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest request){
        UWebsocketsClient *client = new UWebsocketsClient(ws);
        webSockConnections.emplace(ws, client);
        Catflow::onClientConnect(*client);
      });

      hub.onDisconnection([this](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length){
        auto client = webSockConnections.find(ws);
        Catflow::onClientDisconnect(*client->second);
        webSockConnections.erase(client);
        delete client->second;
      });

      // http
      if (Catflow::httpServerPath != "")
        hub.onHttpRequest([this](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
          // on Http request
          auto doc = webDocs.find(req.getUrl().toString());
          if (doc != webDocs.end())
            res->end(doc->second.data(), doc->second.size());
          else {
            trace("got http request for " + req.getUrl().toString() + " -> redirect to index.html ");
            doc = webDocs.find("/index.html");
            if (doc != webDocs.end())
              res->end(doc->second.data(), doc->second.size());
            else {
              string s = "can't find index.html :(";
              res->end(s.data(), s.size());
            }
          }
        });

      hub.onError([this](int type){
        err("err on port " + to_string(type));
      });
    }

    void start() override
    {
      // start server threads
      serverThread = new thread([this](){
        if (hub.listen(Catflow::port)) {
          // run is blocking
          ok("listen for webSocket/http connection on port " + to_string(Catflow::port));

          // cache web files
          if (Catflow::httpServerPath.size() > 0) {
            int filesAmount = 0;
            int filesTotalSize = 0;
            try
            {
              auto path = boost::filesystem::path(Catflow::httpServerPath);
              int cutFromPath = (Catflow::httpServerPath.back() == '/')  ?  path.size()-1 : path.size();

              for (boost::filesystem::recursive_directory_iterator i(path), end; i != end; ++i)
              {
                if (!is_directory(i->path())) {
                  string webDocPath = i->path().string().substr(cutFromPath, i->path().size() - cutFromPath);
                  boost::filesystem::ifstream infile{i->path()};
                  string str{istreambuf_iterator<char>(infile), istreambuf_iterator<char>()};
                  webDocs.emplace(webDocPath, str);
                  filesTotalSize+= str.size();
                  filesAmount++;
                  // trace("cached file: " + webDocPath);
                }
              }
            } catch (std::exception &e) {
              err("can't read cache web files: " + string(e.what()));
            }
            info("cached " + to_string(filesAmount) + " files ("+ to_string((float)filesTotalSize/(1024*1024)) +" Mbyte) from webserver root '" + Catflow::httpServerPath +"'");
          }

          hub.run();
        } else {
          fatal("listen for webSocket/http connection on port " + to_string(Catflow::port));
        }
      });
      ok("created serverWebsocket thread");
    }

    void startBlocking() override
    {
      ok("will start serverWebsocket blocking");
      if (hub.listen(Catflow::port)) {
        // run is blocking
        ok("listen for webSocket connection on port " + to_string(Catflow::port));
        hub.run();
      } else {
      }
    }

    void send(Json packet, Client &destination, PacketIdentifier &packetIdentifier) override
    {
        UWebsocketsClient *dest = dynamic_cast<UWebsocketsClient*>((Client*)&destination);

        string msg = packet.dump();
        dest->websocket->send(msg.data(), msg.size(), uWS::TEXT);
        packetIdentifier.sendDone();
    }

    void sendBroadcast(Json packet) override
    {
      for (auto s : webSockConnections) {
        string msg = packet.dump();
        s.first->send(msg.data(), msg.size(), uWS::TEXT);
      }
    }


  private:
    uWS::Hub hub;
    thread *serverThread;
    map<uWS::WebSocket<uWS::SERVER> *, UWebsocketsClient *> webSockConnections;
    map<string, string> webDocs;
};

#endif //OPENHABAI_APISUWEBSOCKETSSERVER_H
