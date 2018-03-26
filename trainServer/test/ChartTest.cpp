/* 
 * File:   ChartTest.cpp
 * Author: Joshua Johannson
 *
  */

#include <gtest/gtest.h>
#include <api/ApiRoute.h>
#include <api/JsonList.h>
#include <api/ApiJsonObject.h>
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