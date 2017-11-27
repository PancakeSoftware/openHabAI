/* 
 * File:   Controller.h
 * Author: Joshua Johannson
 *
  */
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <json.hpp>
#include <list>
#include "Frontend.h"
#include "dataStructures/DataStructure.h"

using namespace std;
using Json = nlohmann::json;


/*
 * Controller class
 * handles frontend commands
 * central control
 * holds dataStructures List
 */
class Controller
{
  public:
    Controller() {}
    static void loadAll(); // load saved

    static JsonList dataStructures;

    static void onFrontendMessage(Json msg);
};


#endif //CONTROLLER_H
