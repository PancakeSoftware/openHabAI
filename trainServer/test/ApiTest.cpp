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
    int id;
    Person()
    {setJsonParams({{"id", &id},
                    {"age", &age},
                    {"name", &name} });}
};

/*
 * UniversityCourse, contains list of students
 */
class UniversityCourse : public JsonObject, public ApiRoute
{
  public:
    JsonList<Person> students;
    int studentsIdAutoIncrement = 0;

    string courseName;
    int id;

    UniversityCourse(): students(studentsIdAutoIncrement, "", ""), // @TODO set path
                        ApiRoute({{"students", &students}})
    {
      setJsonParams({{"courseName", &courseName},
                     {"id", &id} });
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
        courses(coursesIdAutoIncrement, "build/test", "ApiRouteTest_MyList.json"),
        ApiRoute({  // set sub routes
                     {"courses", &courses}
                 }) {}
};




TEST(ApiRouteTest, progressListAddGet)
{
  RootRoute root;

  /*
   * Add tow courses: SystemParallelProgramming, and Math1
   */
  ApiRequest *request = new ApiRequest(
      Json {{
          {"courses", ""}
      }},
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
          Json{{
                   {"courses", to_string(course)},
                   {"students", ""}
               }},
          "add",
          Json{{"name", name}, {"age", 19}}
      ))->toJson() << endl;


  /*
   * check length of lists
   */
    EXPECT_EQ(2, root.courses.length());
    EXPECT_EQ(3, root.courses.get(0).students.length());
    EXPECT_EQ(3, root.courses.get(1).students.length());


  /*
   * check contend of lists
   */
  EXPECT_TRUE(testCompareJson(
      Json{
          {{"courseName", "SystemParallelProgramming"}, {"id", 0}},
          {{"courseName", "Math1"}, {"id", 1}}
      },
      root.processApi(ApiRequest(
        Json {{
          {"courses", ""}
        }},
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
          Json {{
                    {"courses", course},
                    {"students", ""}
                }},
          "getAll",
          nullptr,
          0))->data
    ));
}