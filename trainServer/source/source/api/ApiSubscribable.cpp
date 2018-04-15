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
    if (processible != nullptr)
        return processible;

    // subscribe
    if (request.what == "subscribe") {
        subscribers.insert(request.client);
        l.info("new subscriber " + request.client.toString());
    }
    if (request.what == "unsubscribe")
        subscribers.erase(request.client);
}

void ApiSubscribable::sendToSubscribers(ApiRequest respond) {
    for (auto sub : subscribers)
        Frontend::send(respond, sub);
}
