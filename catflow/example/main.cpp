/* 
 * simple catflow example
 */
#include <ApiJsonObject.h>
#include <ApiRoute.h>
#include <util/TaskManager.h>
#include <Chart.h>
#include <server/ApiSeasocksServer.h>
#include <server/ApiuWebsocketsServer.h>

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
    JsonList<Student> students;
    ParameterChart chart;

    string courseName;
    int id; // <courseId>

    UniversityCourse():
        chart()
    {
      setSubRoutes({{"students", &students},// mount JsonList<Student> at 'students/'
                    {"chart", &chart}       // mount chart at 'chart/'
                   });

      // set chart inputs[id, name]/outputs[id, name]
      chart.setInputOutputNames({make_pair(0, "day in semester")},
                                {make_pair(0, "students visiting lecture")}
      );
      // function to generate output by input
      chart.setUpdateFunction([](const map<int, float> &inputValues, const vector<int> &outputIds) {
        map<int, float> studentsInLecture;

        if (inputValues.find(0) == inputValues.end())
          return studentsInLecture;

        float dayInSemester = inputValues.at(0); // get input with id 0
        studentsInLecture.emplace(0,
                                  1/(pow(dayInSemester, 2)+0.05) + pow(dayInSemester, 2)); // set output with id 0 to f(x)
        return studentsInLecture;
      });
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
    JsonList<UniversityCourse> courses;

    RootRoute() {
      setSubRoutes({  // set sub routes
        {"courses", &courses} // mount JsonList<UniversityCourse> at 'courses/'
      });
    }
};



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
  Catflow::start<ApiuWebsocketsServer>(
      5555 // webSocket port
  );

  // start blocking taskManager, use for chart updates
  TaskManager::start();

  // on exit
  return EXIT_SUCCESS;
}