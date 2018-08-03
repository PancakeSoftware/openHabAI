/* 
 * File:   $HEADER_FILENAME
 * Author: Joshua Johannson
 *
  */
#ifndef OPENHABAI_UTIL_H
#define OPENHABAI_UTIL_H
#include <string>
#include <vector>
#include <algorithm>
#include <JsonObject.h>
using namespace std;

inline vector<string> intersect(vector<string> &v1, vector<string> &v2) {
  vector<string> v3;
  sort(v1.begin(), v1.end());
  sort(v2.begin(), v2.end());
  set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v3));
  return v3;
}

/**
 * contians vector1 minimal one element of vector2
 * @param vector1
 * @param vector2
 * @return
 */
inline bool contains(vector<string> &vector1, vector<string> vector2) {
  return intersect(vector1, vector2).size() >= 1;
}


class Range: public JsonObject{
  public:
    float from = 0;
    float to = 0;
    int steps = 1;

    void params() override {
      param("from", from);
      param("to", to);
      param("steps", steps);
    }
};

/**
 * interpolate n-dimensional data-grid by given ranges for each dimension
 * @param inputRanges ranges for each dimension (from, to, steps)
 * @param inputData to this vector the data for each dimension is appended, via this param you can set initial dimension data
 * @return list of points(has in each dimension a value) in the grid
 */
inline vector<vector<float>> &_createInputDataGrid(vector<Range>::const_iterator input, vector<Range>::const_iterator inputEnd, vector<vector<float>> &inputDataList, vector<float> inputPoint)
{
  if (input == inputEnd)
  {
    //debug("create dataPoint at "+ Json{inputData}.dump());
    if (!inputPoint.empty())
      inputDataList.push_back(inputPoint); // add new point
    return inputDataList;
  }

  // calc increase step for rangeInput
  float step;
  if (input->steps > 0)
    step = ((input->to - input->from) / input->steps);
  else
    step = 0;
  step = (step > 0) ? step : 0;
  //debug("progress input (perStep+="+to_string(step)+")"+ input->toJson().dump());

  // for each point of range
  for (float i = input->from; i <= input->to; i += step)
  {
    vector<float> nInputPoint = inputPoint;
    nInputPoint.push_back(i);
    _createInputDataGrid(input+1, inputEnd, inputDataList, nInputPoint);
    if (step == 0)
      break;
  }
};

/**
 * interpolate n-dimensional data-grid by given ranges for each dimension
 * @param inputRanges ranges for each dimension (from, to, steps)
 * @return list of points(has in each dimension a value) in the grid
 */
inline vector<vector<float>> createInputDataGrid(vector<Range> &inputRanges) {
  vector<vector<float>> inputDataList{};
  _createInputDataGrid(inputRanges.begin(), inputRanges.end(), inputDataList, vector<float>{});
  return inputDataList;
}

#endif //OPENHABAI_UTIL_H
