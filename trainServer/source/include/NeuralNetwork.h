//
// Created by joshua on 15.07.17.
//

#ifndef NeuralNetwork_H
#define NeuralNetwork_H

#include <mxnet-cpp/MxNetCpp.h>
#include <thread>
#include <json.hpp>
#include <string>
#include <Frontend.h>
#include <json/ApiRoute.h>
using namespace std;
using namespace mxnet::cpp;
class DataStructure;
class Frontend;


class NeuralNetwork : public ApiRouteJson
{
  public:
    DataStructure &structure;
    int id;

    string name, optimizer = "sgd";
    float learnrate = 0;
    int hiddenLayers, neuronsPerLayer;

    static void init(Context *mxContext);
    NeuralNetwork();
    NeuralNetwork(DataStructure *structure);
    NeuralNetwork(DataStructure *structure, Json params);
    ~NeuralNetwork();

    void train();
    void trainInNewThread();
    void joinTrainThread();

    static void shutdown();

    void stopTrain();

    ApiRespond *processApi(ApiRequest request) override;

  private:
    map<std::string, NDArray> graphValues;
    vector<std::string> graphValueNames;
    Symbol symLossOut;

    static Context *ctx;
    thread *trainThread;

    int batchSize;

    int iteration = 0;
    bool trainEnable;

    void printSymbolShapes(map<string, NDArray> map1);

    // Charts
    Frontend::Chart chartProgress;
    Frontend::Chart chartShape;

};


#endif //NeuralNetwork_H
