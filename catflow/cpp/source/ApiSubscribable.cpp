/*
 * File:   ApiSubscribable.cpp
 *
 */

#include "ApiSubscribable.h"
#include <server/ApiServer.h>

Log ApiSubscribable::l("ApiSubscribable");

ApiSubscribable::ApiSubscribable() {
    // subscribers always has to be subset of connections
    Catflow::registerClientList(subscribers);
}

ApiRespond *ApiSubscribable::processApi(ApiRequest request) {
    ApiRespond *processible =  ApiProcessible::processApi(request);

    if (request.client == nullptr && (request.what == "subscribe" || request.what == "unsubscribe")) {
      l.err("can't add subscriber without knowing client");
      return processible;
    }

    // subscribe
    if (request.what == "subscribe") {
        subscribers.insert(request.client);
        l.info("("+routeString+") new subscriber " + request.client->toString() + " (subscribers: "+to_string(subscribers.size())+")");
    }
    if (request.what == "unsubscribe") {
      subscribers.erase(request.client);
      l.info("("+routeString+") remove subscriber " + request.client->toString() + " (subscribers: "+to_string(subscribers.size())+")");
    }

    return processible;
}

void ApiSubscribable::sendToSubscribers(ApiRequest request) {
    for (auto sub : subscribers) {
        Catflow::send(request, *sub);
    }
}

void ApiSubscribable::sendToSubscribers(ApiRequest request, Client &skipSendUpdateTo)
{
    for (auto sub : subscribers) {
        if (sub != &skipSendUpdateTo)
            Catflow::send(request, *sub);
    }
}
