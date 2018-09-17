//
// Created by joshua on 15.07.17.
//

#ifndef NeuralNetwork_H
#define NeuralNetwork_H

#include <mxnet-cpp/MxNetCpp.h>
#include <thread>
#include <string>
#include <Catflow.h>
#include <ApiRoute.h>
#include <Chart.h>
#include <util/TaskManager.h>
using namespace std;
using namespace mxnet::cpp;
class DataStructure;
class Catflow;


class NeuralNetwork : public ApiRouteJson
{
  public:
    DataStructure &structure;
    int id;
    TaskId trainTaskId;

    string name, optimizerType = "sgd";
    float learnrate = 0.005;
    float weightDecay = 0.01;
    int hiddenLayers, neuronsPerLayer;
    Json modelDefinition;

    /* Json keys */
    void params() override { JsonObject::params();
        param("name", name);
        param("id", id);
        param("hidden", hiddenLayers);
        param("neuronsPerHidden", neuronsPerLayer);
        param("learnRate", learnrate);
        param("weightDecay", weightDecay);
        param("optimizer", optimizerType);
        param("modelDefinition", modelDefinition);
    }

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

    /*
     * charts */
    ApiRoute        charts;
    ParameterChart  chartNetworkOutput;
    SeriesChart     chartNetworkProgress;


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

    // train
    Executor* exe{nullptr};
    Optimizer *optimizer{nullptr};
    vector<float> trainDataX;

    void graphBindIo();
};


#endif //NeuralNetwork_H
