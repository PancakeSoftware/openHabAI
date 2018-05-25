/*
 * File:   ApiSubscribable.h
 *
 */
#ifndef OPENHABAI_APISUBSCRIBABLE_H
#define OPENHABAI_APISUBSCRIBABLE_H

#include <util/Log.h>
#include <Catflow.h>
#include "ApiProcessible.h"

/**
 * provides api command: 'subscribe' and 'unsubscribe'
 * @warning do not create object of this class before the main function, because the ApiSubscribable constructor calls a static function of Catflow (its static members have to be constructed before)
 */
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
    void sendToSubscribers(ApiRequest request, Client &skipSendUpdateTo);


  protected:
    set<Client*> subscribers;
    static Log l;
};

#endif //OPENHABAI_APISUBSCRIBABLE_H
