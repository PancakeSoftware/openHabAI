/*
 * File:   ApiSubscribable.h
 *
 */
#ifndef OPENHABAI_APISUBSCRIBABLE_H
#define OPENHABAI_APISUBSCRIBABLE_H

#include <util/Log.h>
#include <Frontend.h>
#include "ApiProcessible.h"


class ApiSubscribable: public virtual ApiProcessible
{
  public:
    ApiSubscribable();

    ApiRespond *processApi(ApiRequest request) override;

    /**
     * send api message to each subscriber
     * @param request
     */
    void sendToSubscribers(ApiRequest request);

    /**
     * send api message to each subscriber
     * @param request
     * @param skipSendUpdateTo not send update to this client
     */
    void sendToSubscribers(ApiRequest request, Client skipSendUpdateTo);


  protected:
    set<Client> subscribers;
    static Log l;
};

#endif //OPENHABAI_APISUBSCRIBABLE_H
