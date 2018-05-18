/* 
 * simple catflow example
 */
#include <ApiJsonObject.h>
#include <ApiRoute.h>
#include <util/TaskManager.h>

/*
 * students in course
 * At: /courses/<courseId>/students/<studentId>/
 */
class Student : public ApiJsonObject
{
  public:
    string name;
    int age;
    int id; // <studentId>

    void params() override
    {
      param("id", id);
      param("age", age);
      param("name", name);
    }
};

/*
 * UniversityCourse, contains list of students
 * At: /courses/<courseId>/
 */
class UniversityCourse : public ApiRouteJson
{
  public:
    JsonList <Student> students;
    int studentsIdAutoIncrement = 0;

    string courseName;
    int id; // <courseId>

    UniversityCourse() :
        students(studentsIdAutoIncrement),
        ApiRouteJson({{"students", &students}}) // mount JsonList<Student> at 'students/'
    {
    }

    void params() override
    {
      param("courseName", courseName);
      param("id", id);
    }
};


/*
 * Root of api, entry-point
 * contains /courses as subRoute
 * At: /
 */
class RootRoute : public ApiRoute
{
  public:
    JsonList <UniversityCourse> courses;

    int coursesIdAutoIncrement = 0;

    RootRoute() :
        courses(coursesIdAutoIncrement),
        ApiRoute({  // set sub routes
                     {"courses", &courses} // mount JsonList<UniversityCourse> at 'courses/'
                 })
    {
    }




/*
 * MAIN
 * start catflow server
 */
int main()
{
  RootRoute rootRoute;
  rootRoute.setStorePath("./example"); // store all in this dir
  rootRoute.restore();                 // restore object-tree from previous run

  Catflow::setApiRootRoute(rootRoute);
  Catflow::start(
      5555, // webSocket port
      8050  // http port, serve web app
  );

  // start blocking taskManager, use for chart updates
  TaskManager::start();
}