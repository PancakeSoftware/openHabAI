#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <json.hpp>
#include <ApiProcessible.h>
#include <JsonList.h>
using namespace std;
using Json = nlohmann::json;

TEST(DebugTest, tryCatch)
{
  try
  {
    throw domain_error("error test");
  } catch (exception& e) {
    cout << "got error: " << e.what() << endl;
  }
}



class SomeClass {
  public:
    int a;
    string b;
};

template<class T, class CLASS>
T getSet(T CLASS::* var, CLASS &obj, T value){
  obj.*var = value;
  return obj.*var ;
};

template<class T, class CLASS>
T get(T CLASS::* var) {
  return var;
};

TEST(DebugTest, classMemberPointer)
{
  SomeClass test;
  EXPECT_EQ(5, getSet(&SomeClass::a, test, 5));
}


TEST(DebugTest, createJsonListObject)
{
  JsonList<ApiJsonObject> jsonList;
}
