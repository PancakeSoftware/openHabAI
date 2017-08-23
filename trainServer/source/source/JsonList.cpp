/* 
 * File:   JsonList.h
 * Author: Joshua Johannson
 *
  */
#include <Frontend.h>
#include <dirent.h>
#include <fstream>
#include "JsonList.h"


#include "util/Filesystem.h"

JsonListItem::JsonListItem(int &idIncrementCounter) : idIncrementRef(idIncrementCounter)
{
}

void JsonListItem::fromJson(Json params)
{
  if (params.find("id") == params.end())
    id = idIncrementRef++;
  else
    id = params["id"];
}

Json JsonListItem::toJson()
{
  return Json {
      {"id", id}
  };
}
bool JsonListItem::save(string path, string fileName)
{
  // check if folder exists
  Filesystem::createDirIfNotExists(path);

  ofstream stream;
  stream.open(path + "/" + fileName);
  cout << "save: " << path << "/" << fileName << endl;
  if (!stream.is_open()) {
    stream.close();
    return false;
  }
  stream << toJson().dump(2);
  stream.close();
  return true;
}
bool JsonListItem::load(string path, string fileName)
{
  ifstream stream;
  stream.open(path + "/" + fileName);
  cout << "load: " << path << "/" << fileName << endl;
  if (!stream.is_open()) {
    stream.close();
    return false;
  }

  Json json(stream);
  fromJson(json);

  stream.close();
  return true;
}
void JsonListItem::loadChilds()
{
}


JsonList::JsonList(const function<JsonListItem *(Json params)> &createItemFunc,
                   string folder,
                   string entryFileName)
    : createItemFunc(createItemFunc), folder(folder), entryFileName(entryFileName)
{
}

void JsonList::progressCommand(Json command)
{
  string type = command["type"];
  string what = command["what"];
  Json data = command["data"];

  // -- GET ------------
  if (type == "get")
  {
    if (data.find("id") == data.end())
      return;

    for (auto el : items)
    {
      if (el->id == (int) data["id"])
      {
        Frontend::respond("ok", el->toJson(), command);
        return;
      }
    }
    string id = data["id"];
    Frontend::respond("error", {{"message", "id " + id + " not found"}}, command);
  }


  if (type == "getAll")
  {
    Json res {};
    for (auto el : items)
      res.push_back(el->toJson());
    Frontend::respond("ok", res, command);
  }


  // -- CREATE ------------
  if (type == "create")
  {
    Filesystem::createDirIfNotExists(this->folder + "/");
    JsonListItem *item = createItemFunc(data);
    Filesystem::createDirIfNotExists(this->folder + "/" + to_string(item->id));
    item->loadChilds();

    if (!item->save(this->folder + "/" + to_string(item->id), entryFileName)) {
      cout << "could not save Item" << endl;
      Frontend::respond("error", {{"message", "can't create " + to_string(item->id)}}, command);
      return;
    }

    items.push_back(item);
    cout << "saved" << endl;

    Frontend::respond("ok", item->toJson(), command);
  }
}

bool JsonList::loadAll()
{
  bool ok = true;

  // for folders in dir
  Filesystem::lsFor(folder, [&](string dirName, bool isDir) {
    if (!isDir)
      return;

    string file = folder + "/" + dirName + "/" + entryFileName;

    ifstream stream;
    stream.open(file);

    if (!stream.is_open()) {
      stream.close();
      ok = false;
    }
    else
    {
      Json json(stream);
      JsonListItem *item = createItemFunc(json);
      if (item->id+1 > item->idIncrementRef)
        item->idIncrementRef = item->id+1;

      item->loadChilds();
      items.push_back(item);

      stream.close();
      cout << "loaded: " << file << endl;
    }

  });


  return ok;
}

JsonListItem *JsonList::get(int id)
{
  for (auto item : items)
    if (item->id == id)
      return item;

  return nullptr;
}
