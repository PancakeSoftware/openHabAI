/* 
 * File:   ApiTest.cpp
 * Author: Joshua Johannson
 *
  */
#include "TestHelper.hpp"
#include <gtest/gtest.h>
#include <api/ApiRoute.h>
#include <string>
#include <api/JsonList.h>
#include <api/ApiJsonObject.h>


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

class MyListL
{
  public:
    JsonList<EntityList> list;
    int entitysIdMax =0;

    MyListL() :
        list(entitysIdMax)
    {}
};


TEST(JsonListTest, add)
{
  MyListL list;
  ApiRequest request;

  request.route = Json {
      {{"compo", ""}}
  };
  request.what = "add";
  request.data = Json{
      {"param", "hello"}
  };
  ApiRequest request2 = request;
  request2.data["param"] = "you";

  cout << "len: " << list.list.length() << endl;

  // add two
  list.list.processApi(request);
  list.list.processApi(request2);

  EXPECT_EQ(2, list.list.length());
  EXPECT_TRUE(testCompareJson(request.data.merge(Json{{"id", 0}}), (*list.list.items.begin()).second->toJson()));
  EXPECT_TRUE(testCompareJson(request2.data.merge(Json{{"id", 1}}), (*++list.list.items.begin()).second->toJson()));
}