#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <json.hpp>
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

TEST(DebugTest, jsonError)
{
  string test = "{\"x\": \"\"d}";
  cout << "will read: " << test << endl;

  Json j;
  try
  {
    j = Json::parse(test);
  } catch (exception& e) {
    cout << "error: " << e.what() << endl;
  }

  cout << j.dump(2) << endl;
}