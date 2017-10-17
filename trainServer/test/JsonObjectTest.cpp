/* 
 * File:   Test
 * Author: Joshua Johannson
 *
  */

#include <gtest/gtest.h>
#include <util/JsonObject.h>


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
  Json val{
      {"value1", 20},
      {"value2", "string"},
      {"value3", true}
  };

  MyObject myObject;
  myObject.fromJson(val);

  EXPECT_EQ(20, myObject.i);
  EXPECT_EQ("string", myObject.s);
  EXPECT_EQ(true, myObject.b);
  EXPECT_EQ(val, myObject.toJson());
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
  EXPECT_EQ(val, myObjectNew.toJson());
}