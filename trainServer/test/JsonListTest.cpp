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

  // add two
  list.processApi(request);
  list.processApi(request2);

  EXPECT_EQ(2, list.length());
  EXPECT_TRUE(testCompareJson(request.data.merge(Json{{"id", 0}}), (*list.items.begin()).second->toJson()));
  EXPECT_TRUE(testCompareJson(request2.data.merge(Json{{"id", 1}}), (*++list.items.begin()).second->toJson()));
}