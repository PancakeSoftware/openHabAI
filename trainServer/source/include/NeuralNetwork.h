//
// Created by joshua on 15.07.17.
//

#ifndef NeuralNetwork_H
#define NeuralNetwork_H

#include <mxnet-cpp/MxNetCpp.h>
#include <thread>
#include <json.hpp>
#include <string>
#include <JsonList.h>
#include <DataStructure.h>
using namespace std;
using namespace mxnet::cpp;


class NeuralNetwork : public JsonListItem
{
  public:
    DataStructure &structure;

    string name, optimizer;
    float learnrate;
    int hiddenLayers, neuronsPerLayer;

    static void init(Context *mxContext);
    NeuralNetwork(DataStructure *structure);
    NeuralNetwork(DataStructure *structure, Json params);
    ~NeuralNetwork();

    void train();
    void trainInNewThread();
    void joinTrainThread();

    static void shutdown();

    void stopTrain();
    Json toJson() override;
    void fromJson(Json params) override;

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

};


#endif //NeuralNetwork_H
