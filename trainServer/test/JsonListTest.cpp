/* 
 * File:   ApiTest.cpp
 * Author: Joshua Johannson
 *
  */
#include "TestHelper.hpp"
#include <gtest/gtest.h>
#include <json/ApiRoute.h>
#include <string>
#include <json/JsonList.h>
#include <json/JsonObject.h>


class EntityList : public JsonObject
{
  public:
    string param;
    int id;

    EntityList()
    {
      setJsonParams({
              {"param", &param},
              {"id", &id}
      });
    }
};

class MyListL
{
  public:
    JsonList<EntityList> list;
    int entitysIdMax =0;

    MyListL() :
        list(entitysIdMax, "build/test", "JsonListTest_MyList.json")
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
  list.list.progressApi(request);
  list.list.progressApi(request2);

  EXPECT_EQ(2, list.list.length());
  EXPECT_TRUE(testCompareJson(request.data.merge(Json{{"id", 0}}), (*list.list.items.begin()).second->toJson()));
  EXPECT_TRUE(testCompareJson(request2.data.merge(Json{{"id", 1}}), (*++list.list.items.begin()).second->toJson()));
}