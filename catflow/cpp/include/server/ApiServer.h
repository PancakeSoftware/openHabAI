/* 
 * File:   ApiServer
 */
#ifndef OPENHABAI_APISERVER_H
#define OPENHABAI_APISERVER_H

#include <util/Log.h>
#include <string>
using namespace std;
struct PacketIdentifier;


/**
 * Abstract class ApiServer
 * Interface for network communication of catflow
 * - implement this class to define your own network backend
 */
class ApiServer: protected Log
{
  public:
    ApiServer()
    :Log("ApiServer"){}

    /**
     * start server non blocking
     */
    virtual void start() = 0;

    /**
     * start server blocking
     */
    virtual void startBlocking() = 0;

    /**
     * send packet to connected client
     * @param packet
     * @param destination
     * @param packetIdentifier call sendDone() on this object after send is complete
     */
    virtual void send(Json packet, Client &destination, PacketIdentifier &packetIdentifier) = 0;

    virtual void sendBroadcast(Json packet) = 0;
};


/**
 * identifier for client who send a request
 */
class Client {
  public:
    Client() {}

    virtual string toString() = 0;

    virtual bool operator<(const Client &other) const = 0;
};

/**
 * dummy void client
 */
class VoidClient: public Client {
  public:
    virtual string toString() override {
      return "Void dummy client";
    };

    virtual bool operator<(const Client &other) const override {
      return true;
    };
};

#endif //OPENHABAI_APISERVER_H
