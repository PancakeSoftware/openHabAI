/* 
 * File:   Controller.h
 * Author: Joshua Johannson
 *
  */
#include "Controller.h"
#include <NeuralNetwork.h>

JsonList Controller::dataStructures(DataStructure::create, "dataStructures", "structure.json");




void Controller::loadAll()
{
  dataStructures.loadAll();
}

void Controller::onFrontendMessage(Json msg)
{
  string type = msg["type"];
  string what = msg["what"];
  Json data = msg["data"];

  // -- DO ------------
  if (type == "do")
  {
    if (what == "startTrain")
    {
      cout << "=>> start train" << endl;
      ((NeuralNetwork*)((DataStructure*)dataStructures.get(data["datastructure"]))->networks.get(data["networkId"]))->train();

      Frontend::send("message", {{"message", "kill old training"}});

      cout << "-- WILL KILL OLD" << endl;

    }

    // feedback
    Frontend::respond("ok", data);


    if (what == "stopTrain")
    {
      cout << "=>> stop train" << endl;


      // feedback
      Frontend::respond("ok", data);
    }
  }


  // -- LISTS ------------
  if (what == "network") {
    DataStructure *structure = ((DataStructure*)dataStructures.get(
        atoi(data["datastructure"].get<string>().c_str())
    ));

    if (structure != nullptr)
    {
      structure->networks.progressCommand(msg);
    }
  }

  if (what == "datastructure")
    dataStructures.progressCommand(msg);
}

