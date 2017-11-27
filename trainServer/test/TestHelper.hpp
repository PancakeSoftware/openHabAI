/* 
 * File:   TestHelper
 * Author: Joshua Johannson
 *
  */
#ifndef OPENHABAI_TESTHELPER_H
#define OPENHABAI_TESTHELPER_H

#include <gtest/gtest.h>
#include <json.hpp>
using Json = nlohmann::json;
using namespace std;

inline testing::AssertionResult testCompareJson(Json const &expect, Json const &actual)
{
  if (expect == actual)
  {
    return testing::AssertionSuccess();
  }
  else
  {
    return testing::AssertionFailure() << "\nEXPECTED:\n" << expect.dump(2) << "\nbut ACTUAL:\n" << actual.dump(2);
  }
}

#endif //OPENHABAI_TESTHELPER_H
