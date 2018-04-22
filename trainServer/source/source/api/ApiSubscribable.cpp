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
        l.info("new subscriber " + request.client.toString());
    }
    if (request.what == "unsubscribe")
        subscribers.erase(request.client);

    return processible;
}

void ApiSubscribable::sendToSubscribers(ApiRequest request) {
    for (auto sub : subscribers)
        Frontend::send(request, sub);
}
