#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <json.hpp>
#include <api/ApiProcessible.h>
#include <dataStructures/DataStructure.h>
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

TEST(DebugTest, delete)
{
  DataStructure *structure = new FunctionDataStructure();
  delete structure;

}