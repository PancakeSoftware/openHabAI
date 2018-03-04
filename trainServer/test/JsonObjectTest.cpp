/* 
 * File:   Test
 * Author: Joshua Johannson
 *
 */

#include <gtest/gtest.h>
#include "TestHelper.hpp"
#include <json/JsonObject.h>


class  MyObject : public JsonObject
{
  public:
    int i = 10;
    string s = "";
    bool b = false;

    MyObject()
    {
      setJsonParams({{"value1", &i},
                     {"value2", &s},
                     {"value3", &b}});
    };
};


TEST(JsonObjectTest, fromToJson)
{
  Json valIn{
      {"value1", 20},
      {"value2", "string"},
      {"value3", true},
      {"dontHave", "error"}
  };

  Json valOut{
      {"value1", 20},
      {"value2", "string"},
      {"value3", true},
  };

  MyObject myObject;
  myObject.fromJson(valIn);

  EXPECT_EQ(20, myObject.i);
  EXPECT_EQ("string", myObject.s);
  EXPECT_EQ(true, myObject.b);
  EXPECT_EQ(valOut, myObject.toJson());
}


TEST(JsonObjectTest, saveLoad)
{
  Json val{
      {"value1", 20},
      {"value2", "string"},
      {"value3", true}
  };

  MyObject myObject;
  myObject.fromJson(val);
  myObject.save("build/test", "testJsonObject.json");

  MyObject myObjectNew;
  myObjectNew.load("build/test", "testJsonObject.json");


  EXPECT_EQ(20, myObjectNew.i);
  EXPECT_EQ("string", myObjectNew.s);
  EXPECT_EQ(true, myObjectNew.b);
  EXPECT_TRUE(testCompareJson(val, myObjectNew.toJson()));
}