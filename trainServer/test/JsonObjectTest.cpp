/* 
 * File:   Test
 * Author: Joshua Johannson
 *
 */

#include <gtest/gtest.h>
#include "TestHelper.hpp"
#include <api/ApiJsonObject.h>


class  MyObject : public ApiJsonObject
{
  public:
    int i = 10;
    string s = "";
    bool b = false;

    void params() override {JsonObject::params();
      param("value1", i);
      param("value2", s);
      param("value3", b);
    }
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
  myObject.save("../test/", "testJsonObject.json");
  cout << "save finish" << endl;


  MyObject myObjectNew;

  cout << "now load" << endl;
  myObjectNew.load("../test/", "testJsonObject.json");


  EXPECT_EQ(20, myObjectNew.i);
  EXPECT_EQ("string", myObjectNew.s);
  EXPECT_EQ(true, myObjectNew.b);
  EXPECT_TRUE(testCompareJson(val, myObjectNew.toJson()));
}



class SimpleObj: public JsonObject{
  public:
    float from;
    float to;
    int id;
    string name = "--";
    vector<int> vec;
    vector<SimpleObj> objVec;

    SimpleObj(){
      cout << endl << "[SimpleObj] '"<< name <<"' create" << endl;

      cout << "pointer from: " << static_cast<void*>(&from) << endl;
      cout << "pointer name: " << static_cast<void*>(&name) << endl;
    }

    void params() override
    {
      JsonObject::params();
      cout << "[SimpleObj] defineParams() " << endl;

      param("from", from);
      param("to", to);
      param("id", id);
      param("name", name);
      param("vec", vec);
      param("objVec", objVec);
    }


    /*
    SimpleObj(const SimpleObj &old): JsonObject(old) {
      cout << "[SimpleObj] '"<< name <<"'<-'"<< old.name << "' copy" << endl;
    }

    SimpleObj& operator =(const SimpleObj &src) {
      JsonObject::operator=(src);
      cout << "[SimpleObj] '"<< name <<"'<-'"<< src.name << "' copy assign" << endl;
    }

    ~SimpleObj() {
      cout << "[SimpleObj] '"<< name <<"' destroy" << endl;
    }*/
};

class As {
  public:
    string name = "--";
    As(string name) {
      this->name = name;
      cout << "Constructor " << name << endl;
    }
    As(const As& old) {
      cout << "Copy "<< name << "  other: " << old.name << endl;
      //*this = old;
    }

    As& operator=(As const &r) {
      cout << "Copy assign "<< name << "  other: " << r.name << endl;
      //*this = r;
      return *this;
    }

    ~As() {
      cout << "delete "<< name << endl;
    }
};

TEST(JsonObjectTest, simpleJsonObject)
{
  {
    As a("a");
    As b("b");
    b = a;
    cout << "Name: " << b.name << endl;
    As c = a;
  }



  Json json{
      {"from", 1.0},
      {"to", 4.0},
      {"id",2},
      {"name", "outer"},
      {"vec", {1,2,3,4}},
      {"objVec", {{
          {"from", 99.0},
          {"to", 999.0},
          {"id",222},
          {"name", "inner"},
          {"vec", {122,222,322,422}},
          {"objVec", Json::array()}
      }}
      }
  };

  SimpleObj obj;
  obj.params();
  obj.name = "OLD";

  cout << endl << "======= fromJson ==" << endl;
  obj = json;



  //vector<string> a;
  //a = json.get<vector<string>>();

  cout << endl << "======= toJson ==" << endl;
  cout << "from: " << obj.from << " name:" << obj.name << endl;

  Json out = obj;
  EXPECT_TRUE(testCompareJson(json, out));
  cout << "Json: " << out.dump(2) << endl;
}