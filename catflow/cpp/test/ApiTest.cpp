/* 
 * File:   ApiTest.cpp
 * Author: Joshua Johannson
 *
  */

#include <gtest/gtest.h>
#include <ApiRoute.h>
#include <JsonList.h>
#include <ApiJsonObject.h>
#include "TestHelper.hpp"
#include <sstream>


// == test api =====================================
// Api path: /courses/<courseId>/students/<studentId>

/*
 * students in course
 */
class Person: public ApiJsonObject
{
  public:
    string name;
    int age;
    int id; // <studentId>

    void params() override { JsonObject::params();
      param("id", id);
      param("age", age);
      param("name", name);
    }
};

/*
 * UniversityCourse, contains list of students
 */
class UniversityCourse : public ApiRouteJson
{
  public:
    JsonList<Person> students{};

    string courseName;
    int id; // <courseId>

    UniversityCourse() {
      setSubRoutes({{"students", &students}});
    }

    void params() override {JsonObject::params();
      param("courseName", courseName);
      param("id", id);
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
    JsonList<UniversityCourse> courses{};

    RootRoute() {
      setSubRoutes({  // set sub routes
                       {"courses", &courses} // /courses/
                   });
    }
};




TEST(ApiRouteTest, progressListAddGetRemove)
{
  RootRoute root;
  root.setStorePath("../test/apiTest/");

  /*
   * Add tow courses: SystemParallelProgramming, and Math1
   */
  ApiRequest *request = new ApiRequest(
      "/courses",
      "add",
      Json {
          {"courseName", "SystemParallelProgramming"}
      }
  );
  EXPECT_EQ("ok", root.processApi(*request)->toJson()["what"]);
  request->data["courseName"] = "Math1";
  root.processApi(*request);

  /*
   * Add 3 students to each course: Hans, Peter, Max
   */
  for (int course : {0, 1})
    for (string name : {"hans", "peter", "max"})
      EXPECT_EQ("ok",
                root.processApi(ApiRequest(
                    "courses/" +to_string(course)+ "/students",
                    "add",
                    Json{{"name", name}, {"age", 19}}
                ))->toJson()["what"]
      );



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
  rootRestored.setStorePath("../test/apiTest/");
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
        "/courses/",
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
          "/courses/"+course+"/students/",
          "getAll",
          nullptr,
          0))->data
    ));


  /*
   * remove test
   */
  root.processApi(ApiRequest(
      "courses/0",
      "remove"
  ));

  EXPECT_TRUE(testCompareJson(
      Json{
          {{"courseName", "Math1"}, {"id", 1}}
      },
      root.processApi(ApiRequest(
          "/courses/",
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


struct Abc {
    int a;
    string b;
};

void to_json(Json & j, const Abc& p) {
  j = Json{p.b, p.a};
}

void from_json(const Json & j, Abc& p) {
  p.b = j.begin().key();
  p.a = j.begin().value();
}

TEST(VectorTest, copy)
{
  vector<Abc> vector1{{0, "du"}};
  Json json1 = vector1;
  cout << json1.dump(2) << endl;

}

TEST(ApiMessageRoute, parseRoute)
{
  ApiMessageRoute route = ApiMessageRoute("/////courses/0//students/1///");
  EXPECT_EQ("/courses/0/students/1/", route.toString());
  route.push("hey");
  route.push("you");
  EXPECT_EQ("/courses/0/students/1/hey/you/", route.toString());
  EXPECT_EQ(false, route.isEmpty());

  EXPECT_EQ("courses", route.pop());
  EXPECT_EQ("0", route.pop());
  EXPECT_EQ("students", route.pop());
  EXPECT_EQ("/1/hey/you/", route.toString());

  EXPECT_EQ("1", route.pop());
  EXPECT_EQ("hey", route.pop());
  EXPECT_EQ("you", route.pop());

  EXPECT_EQ(true, route.isEmpty());
  EXPECT_EQ("", route.pop());

}