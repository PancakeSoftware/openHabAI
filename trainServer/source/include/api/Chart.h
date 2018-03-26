/* 
 * File:   Chart.h
 * Author: Joshua Johannson
 *
 */
#ifndef OPENHABAI_CHART_H
#define OPENHABAI_CHART_H

#include "ApiJsonObject.h"
#include "Frontend.h"
struct RangeParam;


/*
 * Chart class
 */
class Chart: public ApiJsonObject
{
  public:
    Chart();

    /*
     * Json keys */
    void params() override { ApiJsonObject::params();
    }

    ApiRespond *processApi(ApiRequest request) override;

    /**
     * send change data to subscribers
     */
    void pushUpdate();

    ~Chart() {
      Frontend::unRegisterWebsocketList(subscribers);
    }

  protected:
    set<WebSocket*> subscribers;
};

/*
 * chart for multidimensional data
 * all is updated
 */
class ParameterChart: public Chart
{
  public:
    ParameterChart();

    /*
     * Json keys */
    void params() override { Chart::params();
      param("fixedInputs", fixedInputs);
      param("rangeInputs", rangeInputs);
      param("outputs", outputs);
    }

    ApiRespond *processApi(ApiRequest request) override;

  private:
    vector<int> fixedInputs;
    vector<RangeParam> rangeInputs;
    vector<int> outputs;
};


/*
 * chart for 2D data
 * x-axis = time
 * add dataPoints over time
 */
class TimeChart: public Chart
{

};


class RangeParam: public JsonObject{
  public:
    float from;
    float to;
    int id;

    RangeParam() {

    }

    void params() override { JsonObject::params();
      param("from", from);
      param("to", to);
      param("id", id);
    }
};

#endif //OPENHABAI_CHART_H
