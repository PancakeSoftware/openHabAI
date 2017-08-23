/* 
 * File:   JsonList.h
 * Author: Joshua Johannson
 *
  */
#ifndef JSONLIST_H
#define JSONLIST_H

#include <json.hpp>
#include <string>
#include <list>
using Json = nlohmann::json;
using namespace std;
class JsonListItem;

/*
 * JsonList class
 * elements updatable via json (get, getAll, create)
 */
class JsonList
{
  public:
    function<JsonListItem* (Json params)> createItemFunc;
    list<JsonListItem*> items;
    string folder;
    string entryFileName;

    JsonList(const function<JsonListItem *(Json params)> &createItemFunc,
                 string folder,
                 string entryFileName);
    bool loadAll();
    JsonListItem* get(int id);

    void progressCommand(Json command); // progress command -> send request
};

class JsonListItem
{
  public:
    int id;
    int &idIncrementRef;

    JsonListItem(int &idIncrementCounter);
    JsonListItem(int &idIncrementCounter, string path, string filename);

    virtual Json toJson();
    virtual void fromJson(Json params);
    virtual bool save(string path, string fileName);

    virtual bool load(string path, string fileName);
    virtual void loadChilds();
};



#endif //JSONLIST_H
