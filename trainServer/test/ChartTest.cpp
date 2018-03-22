/* 
 * File:   ChartTest.cpp
 * Author: Joshua Johannson
 *
  */

#include <gtest/gtest.h>
#include <json/ApiRoute.h>
#include <json/JsonList.h>
#include <json/JsonObject.h>
#include "TestHelper.hpp"


// == test chart =====================================
class ApiRoot : public ApiRoute
{
  public:
    ApiRoute test;

    ApiRoot() : ApiRoute({{"test", &test}}) {};
};

TEST(ChartTest, chart)
{

}