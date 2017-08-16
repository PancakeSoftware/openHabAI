//
// Created by joshua on 15.07.17.
//

#ifndef NeuralNetwork_H
#define NeuralNetwork_H

#include <mxnet-cpp/MxNetCpp.h>
#include <thread>

using namespace std;
using namespace mxnet::cpp;


class NeuralNetwork
{
  public:
    NeuralNetwork(int inputNeuronsLen, int outputNeuronsLen);
    ~NeuralNetwork();

    void train();
    void trainInNewThread();
    void joinTrainThread();

    void generate(int hiddenLayersLen, int neuronsPerLayerLen);
    static void shutdown();

    void stopTrain();
  private:
    map<std::string, NDArray> graphValues;
    vector<std::string> graphValueNames;
    Symbol symLossOut;

    Context ctx;
    thread *trainThread;

    int batchSize;

    int iteration = 0;
    bool trainEnable;

    void printSymbolShapes(map<string, NDArray> map1);

};


#endif //NeuralNetwork_H
