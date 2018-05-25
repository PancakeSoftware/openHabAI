/* 
 * File:   ApiTest.cpp
 * Author: Joshua Johannson
 *
  */
#include "TestHelper.hpp"
#include <gtest/gtest.h>
#include <ApiRoute.h>
#include <string>
#include <JsonList.h>
#include <ApiJsonObject.h>


class EntityList : public ApiJsonObject
{
  public:
    string paramS;
    int id;

    void params() override {JsonObject::params();
      param("param", paramS);
      param("id", id);
    }
};



TEST(JsonListTest, add)
{
  JsonList<EntityList> list;
  ApiRequest request;

  request.route.fromString("");
  request.what = "add";
  request.data = Json{
      {"param", "hello"}
  };
  ApiRequest request2 = request;
  request2.data["param"] = "you";

  cout << "len: " << list.length() << endl;

  // add three
  list.processApi(request);
  list.processApi(request2);
  list.add(EntityList());
  list.get(2).paramS = "third";

  EXPECT_EQ(3, list.length());
  EXPECT_EQ(3, list.processApi(ApiRequest("", "get"))->data["length"]);

  EXPECT_EQ("can't set jsonObject key 'length' because of JsonObjectException : can't set length of list",
            list.processApi(ApiRequest("", "update", Json{{"length", 5}}))->data["message"]);

  EXPECT_TRUE(testCompareJson(request.data.merge(Json{{"id", 0}}), (*list.items.begin()).second->toJson()));
  EXPECT_TRUE(testCompareJson(request2.data.merge(Json{{"id", 1}}), (*++list.items.begin()).second->toJson()));
  EXPECT_TRUE(testCompareJson(Json{{"id", 2}, {"param", "third"}}, list.items.find(2)->second->toJson()));
}

TEST(JsonListTest, subscribePushUpdates)
{
  JsonList<EntityList> list;

  // subscribe
  ApiRequest request("", "subscribe");
  request.client = new VoidClient;
  list.processApi(request);

  list.add(EntityList());
  list.get(0).paramS = "element";
  list.remove(0);

  EXPECT_EQ(2, Catflow::requestsToSend.size());
  EXPECT_TRUE(testCompareJson(ApiRequest("0", "add", Json{{"id", 0}, {"param", ""}}).toJson(), Catflow::requestsToSend.front().second.toJson()));
  EXPECT_TRUE(testCompareJson(ApiRequest("0", "remove").toJson(), next(Catflow::requestsToSend.begin())->second.toJson()));

  // cleanup
  Catflow::requestsToSend.clear();
}