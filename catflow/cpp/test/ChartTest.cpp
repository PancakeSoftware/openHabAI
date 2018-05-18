/* 
 * File:   ChartTest.cpp
 * Author: Joshua Johannson
 *
  */

#include <gtest/gtest.h>
#include <ApiRoute.h>

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