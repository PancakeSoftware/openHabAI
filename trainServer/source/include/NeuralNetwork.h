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
using namespace std;
using namespace mxnet::cpp;
class DataStructure;
class Catflow;


class NeuralNetwork : public ApiRouteJson
{
  public:
    DataStructure &structure;
    int id;

    string name, optimizer = "sgd";
    float learnrate = 0;
    int hiddenLayers, neuronsPerLayer;

    /* Json keys */
    void params() override { JsonObject::params();
        param("name", name);
        param("id", id);
        param("hidden", hiddenLayers);
        param("neuronsPerHidden", neuronsPerLayer);
        param("learnRate", learnrate);
        param("optimizer", optimizer);
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
    ParameterChart  chartProgressT;

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
    Catflow::Chart chartProgress;
    Catflow::Chart chartShape;

};


#endif //NeuralNetwork_H
