/* 
 * File:   ApiMessage.h
 * Author: Joshua Johannson
 *
  */
#ifndef OPENHABAI_APIMESSAGE_H
#define OPENHABAI_APIMESSAGE_H

#include <json.hpp>
#include <string>
using Json = nlohmann::json;
using namespace std;

struct ApiRequest
{
    Json route;
    string what;
    Json data;
    int resondId = -1;
    bool respondWanted = false;
};

#endif //OPENHABAI_APIMESSAGE_H
