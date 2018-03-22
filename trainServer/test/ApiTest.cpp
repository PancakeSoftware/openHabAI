/* 
 * File:   ApiTest.cpp
 * Author: Joshua Johannson
 *
  */

#include <gtest/gtest.h>
#include <json/ApiRoute.h>
#include <json/JsonList.h>
#include <json/JsonObject.h>
#include "TestHelper.hpp"


// == test api =====================================
// Api path: /courses/<courseId>/students/<studentId>

/*
 * students in course
 */
class Person: public JsonObject
{
  public:
    string name;
    int age;
    int id; // <studentId>
    Person()
    {
      addJsonParams({{"id", &id},
                     {"age", &age},
                     {"name", &name}});}
};

/*
 * UniversityCourse, contains list of students
 */
class UniversityCourse : public ApiRouteJson
{
  public:
    JsonList<Person> students;
    int studentsIdAutoIncrement = 0;

    string courseName;
    int id; // <courseId>

    UniversityCourse(): students(studentsIdAutoIncrement), // @TODO set path
                        ApiRouteJson({{"students", &students}})
    {
      addJsonParams({{"courseName", &courseName},
                     {"id", &id}});
    }
};


/*
 * At /
 * Root of api, entry-point
 * contains /courses as subRoute
 */
class RootRoute : public ApiRoute
{
  public:
    JsonList<UniversityCourse> courses;
    int coursesIdAutoIncrement = 0;

    RootRoute() :
        courses(coursesIdAutoIncrement),
        ApiRoute({  // set sub routes
                     {"courses", &courses}
                 }) {}
};




TEST(ApiRouteTest, progressListAddGet)
{
  RootRoute root;
  root.setStorePath("../test/apiTest");

  /*
   * Add tow courses: SystemParallelProgramming, and Math1
   */
  ApiRequest *request = new ApiRequest(
      Json {
          {{"courses", ""}}
      },
      "add",
      Json {
          {"courseName", "SystemParallelProgramming"}
      }
  );
  cout << "ADD course: " << root.processApi(*request)->toJson() << endl;
  request->data["courseName"] = "Math1";
  root.processApi(*request);

  /*
   * Add 3 students to each course: Hans, Peter, Max
   */
  for (int course : {0, 1})
    for (string name : {"hans", "peter", "max"})
      cout << "ADD:  " << root.processApi(ApiRequest(
          Json{
              {{"courses", to_string(course)}},
              {{"students", ""}}
          },
          "add",
          Json{{"name", name}, {"age", 19}}
      ))->toJson() << endl;



  /*
   * check length of lists
   */
    EXPECT_EQ(2, root.courses.length());
    EXPECT_EQ(3, root.courses.get(0).students.length());
    EXPECT_EQ(3, root.courses.get(1).students.length());


  /* restored
   * check length of lists
   */
  root.store();
  RootRoute rootRestored;
  rootRestored.setStorePath("../test/apiTest");
  rootRestored.restore();
  EXPECT_EQ(2, rootRestored.courses.length());
  EXPECT_EQ(3, rootRestored.courses.get(0).students.length());
  EXPECT_EQ(3, rootRestored.courses.get(1).students.length());


  /*
   * check contend of lists
   */
  EXPECT_TRUE(testCompareJson(
      Json{
          {{"courseName", "SystemParallelProgramming"}, {"id", 0}},
          {{"courseName", "Math1"}, {"id", 1}}
      },
      root.processApi(ApiRequest(
        Json {
            {{"courses", ""}}
        },
        "getAll",
        nullptr,
        0))->data
  ));

  for (string course : {"0", "1"})
    EXPECT_TRUE(testCompareJson(
      Json{
          {{"name", "hans"}, {"id", 0}, {"age", 19}},
          {{"name", "peter"}, {"id", 1}, {"age", 19}},
          {{"name", "max"}, {"id", 2}, {"age", 19}}
      },
      root.processApi(ApiRequest(
          Json {
              {{"courses", course}},
              {{"students", ""}}
          },
          "getAll",
          nullptr,
          0))->data
    ));
}

TEST(JsonArrayTest, removeFromArray)
{
  Json test{
                {{"courses", "a"}},
                {{"students", ""}}
            };
  cout << "Before: " << test.dump(2) << endl;

  test.erase(test.begin());
  cout << test.dump(2) << endl;
  EXPECT_EQ(1, test.size());
}
