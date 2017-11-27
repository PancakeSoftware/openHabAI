/* 
 * File:   ApiTest.cpp
 * Author: Joshua Johannson
 *
  */

#include <gtest/gtest.h>
#include <json/ApiRoute.h>
#include <json/JsonList.h>
#include <json/JsonObject.h>
#include "TestHelper.hpp"


// == test api =====================================

class Entity : public JsonObject
{
  public:
    string param;

    Entity()
    {
      setJsonParams({ {"param", param} });
    }
};

/*
 * At /myList
 *    /myList/<entity>
 */

class  MyList : public ApiRouteWithEntities<Entity>
{
  public:
    JsonList<Entity> list;
    int idMax =0;

    MyList() :
        ApiRouteWithEntities(list),
        list(idMax, "build/test", "ApiRouteTest_MyList.json"){}
};

/*
 * At /
 * Root of api, entry-point
 */
class RootRoute : public ApiRoute
{
  public:
    MyList myList;

    RootRoute() :
        ApiRoute({&myList}) {}
};




TEST(ApiRouteTest, progressListAdd)
{
  RootRoute root;
  root.progressApi(
      Json {
          {"myList", ""}
      },
      "add",
      Json {
          {"param", "hello"}
      }
  );

  EXPECT_EQ(1, root.myList.list.length());
  if (root.myList.list.items.begin() == root.myList.list.items.end())
    return;
  EXPECT_TRUE(testCompareJson(Json{{"", ""}}, (*root.myList.list.items.begin()).second->toJson()));
}