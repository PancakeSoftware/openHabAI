/*
 * File:   ApiSubscribable.cpp
 *
 */

#include "api/ApiSubscribable.h"

Log ApiSubscribable::l("ApiSubscribable");

ApiSubscribable::ApiSubscribable() {
    // subscribers always has to be subset of connections
    Frontend::registerClientList(subscribers);
}

ApiRespond *ApiSubscribable::processApi(ApiRequest request) {
    ApiRespond *processible =  ApiProcessible::processApi(request);

    // subscribe
    if (request.what == "subscribe") {
        subscribers.insert(request.client);
        l.info("("+routeString+") new subscriber " + request.client.toString() + " (subscribers: "+to_string(subscribers.size())+")");
    }
    if (request.what == "unsubscribe") {
      subscribers.erase(request.client);
      l.info("("+routeString+") remove subscriber " + request.client.toString() + " (subscribers: "+to_string(subscribers.size())+")");
    }

    return processible;
}

void ApiSubscribable::sendToSubscribers(ApiRequest request) {
    for (auto sub : subscribers)
        Frontend::send(request, sub);
}
