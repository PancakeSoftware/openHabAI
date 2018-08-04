#include <Catflow.h>
#include <util/TaskManager.h>
#include <dataStructures/DataStructure.h>
#include "NeuralNetwork.h"

Context *NeuralNetwork::ctx;


void NeuralNetwork::init(Context *mxContext)
{
  ctx = mxContext;
}


// constructor
NeuralNetwork::NeuralNetwork()
    : NeuralNetwork(nullptr)
{}

NeuralNetwork::NeuralNetwork(DataStructure *structure, Json params)
    : NeuralNetwork(structure)
{
  fromJson(params);
}

NeuralNetwork::NeuralNetwork(DataStructure *structure)
    : ApiRouteJson(),
      chartNetworkOutput(),
      chartNetworkProgress(),
      charts(),
      structure(*structure)
{
  setLogName("NETWORK");

  /*
   * onw actions
   */
  addAction("startTrain", [this](ApiRequest request) -> ApiRespond*
  {
    // not already training
    if (!trainTaskId.isValid()) {
      train();
      return new ApiRespondOk(request);
    }
    else
      return new ApiRespondError("training is already running", request);
  });
  addAction("stopTrain", [this](ApiRequest request) -> ApiRespond*
  {
    if (TaskManager::removeTaskRepeating(trainTaskId))
      return new ApiRespondOk(request);
    else
      return new ApiRespondError("can't stop task", request);
  });
  addAction("resetModel", [this](ApiRequest request)
  {
    TaskManager::addTaskOnceOnly([this] {
    // -- INIT ALL random  ----
    auto initializer = Uniform(0.01);
      for (auto& arg : graphValues) {
        // arg.first is parameter name, and arg.second is the value
        initializer(arg.first, &arg.second);
        //arg.second = 1;
      }

      // re display
      chartNetworkProgress.clearData();
      chartNetworkOutput.recalcData(); // @TODO: non blocking
    }, this);

    return new ApiRespondOk(request);
  });

  /*
   * mount charts */
  setSubRoutes({{"charts", &charts}});
  charts.setSubRoutes({{"netOutput", &chartNetworkOutput},
                       {"progress", &chartNetworkProgress}});

  /*
   * charts update functions */
  chartNetworkOutput.setInputOutputNames({"x"}, {"y"});
  chartNetworkOutput.setUpdateFunction([this] (const map<int, float> &inputValues, const vector<int> &outputIds) {
    map<int, float> out;
    out.emplace(0, inputValues.find(0)->second);
    //for (int id: outputIds)
    //  out[id] = inputValues[0]+inputValues[1]+inputValues[2]+inputValues[3] * id;
    return out;
  });
  chartNetworkProgress.setInputOutputNames({"iteration"}, {"rmse-loss", "mse-loss"});

  // clear display



  /* --------------------------------------
   * Graph
   */
  auto numHidden = 40;
  auto symX       = Symbol::Variable("x");
  auto symLabel   = Symbol::Variable("label");


  auto symL1 = tanh( FullyConnected(
      "fullyConnected1",
      symX,
      Symbol::Variable("weight1"),
      Symbol::Variable("bias1"),
      numHidden   /* Num hidden */
  ));

  auto symL2 = tanh( FullyConnected(
      "fullyConnected2",
      symL1,
      Symbol::Variable("weight2"),
      Symbol::Variable("bias2"),
      numHidden   /* Num hidden */
  ));

  auto symL3 = tanh( FullyConnected(
      "fullyConnected3",
      symL2,
      Symbol::Variable("weight3"),
      Symbol::Variable("bias3"),
      numHidden   /* Num hidden */
  ));


  auto symOut = tanh( FullyConnected(
      "fullyConnected4",
      symL3,
      Symbol::Variable("weightLast"),
      Symbol::Variable("biasLast"),
      1   /* Num hidden */
  ));


  symLossOut = LinearRegressionOutput(
      "linearRegression",
      symOut,
      symLabel
  );

  graphBindIo();
}

void NeuralNetwork::graphBindIo()
{
  // set batch size
  batchSize = structure.getDataBatchIndices();

  /* --------------------------------------
   * Graph Value Arrays
   */

  graphValues = {
      {"x", NDArray(Shape(batchSize /* batch */, 1 /*inputs*/), *ctx)},
      {"label", NDArray(Shape(batchSize, 1), *ctx)}
  };


  // inter args
  symLossOut.InferArgsMap(*ctx, &graphValues, graphValues);
  graphValueNames = symLossOut.ListArguments();


  // -- INIT ALL  ----
  auto initializer = Uniform(0.01);
  for (auto& arg : graphValues) {
    // arg.first is parameter name, and arg.second is the value
    initializer(arg.first, &arg.second);
    //arg.second = 1;
  }

  printSymbolShapes(graphValues);
}


// -- TRAIN -----------------------------------------
void NeuralNetwork::train()
{
  /* --------------------------------------
  * Train
  */
  if (exe != nullptr)
    delete exe;
  if (optimizer != nullptr)
    delete optimizer;

  exe = symLossOut.SimpleBind(*ctx, graphValues, map<std::string, NDArray>()); //, graphGradientOps);

  // setup optimizer
  optimizer = OptimizerRegistry::Find("sgd");
  optimizer
      ->SetParam("rescale_grad", 1.0)
      ->SetParam("lr", learnrate)       // learn rate
      ->SetParam("wd", weightDecay);    // weight decay


  /* -----------------------------------
   * Generate Train data
   */
  trainDataX.clear();
  vector<float> y;

  // get train data
  auto data = structure.getDataBatch(0, batchSize);
  debug("data " + Json{data}.dump());
  for (auto record : data) {
    trainDataX.push_back(record.first[0]); // first x
    y.push_back(record.second[0]); // first y
  }
  debug("trainDataX size: " + to_string(trainDataX.size()));

  /*
  int index = 0;
  for (float i = 0; trainDataX.size() < batchSize; i+= 0.33) // until x=10
  {
    trainDataX.push_back(i);
    y.push_back(sin(i*1)*pow(i+2, 2)*0.012);
    index++;
  }*/

  // copy train-data to compute-device
  graphValues["x"].SyncCopyFromCPU(trainDataX);
  graphValues["label"].SyncCopyFromCPU(y);

  cout << endl;
  cout << "X("<< graphValues["x"].Size() <<"): " << graphValues["x"] << endl;
  cout << "L("<< graphValues["label"].Size() <<"): " << graphValues["label"] << endl;


  // display function
  vector<float> xs;
  vector<float> ys;
  graphValues["label"].SyncCopyToCPU(&ys, batchSize);
  graphValues["x"].SyncCopyToCPU(&xs, batchSize);
  //debug("x: " + Json{xs}.dump());
  //debug("label: " + Json{ys}.dump());


  chartNetworkOutput.setUpdateFunction([this] (const map<int, float> &inputValues, const vector<int> &outputIds) {
    Executor* exeChart = symLossOut.SimpleBind(*ctx, graphValues, map<std::string, NDArray>()); //, graphGradientOps);

    map<int, float> out;

    vector<float> x;

    for (float i = 0; i < batchSize; i++)
    {
      x.push_back(inputValues.find(0)->second);
    }

    graphValues["x"].SyncCopyFromCPU(x);
    exeChart->Forward(false);


    vector<float> ys;
    exeChart->outputs[0].SyncCopyToCPU(&ys, batchSize);
    out.emplace(0, ys[0]);

    delete exeChart;

    return out;
  });


  /* --------------------
   * train loop
   */
  cout << endl << "-- Train ----------------" << endl;
  debug("learnRate: " + to_string(learnrate) + "  weightDecay: " + to_string(weightDecay));
  trainEnable = true;


  trainTaskId = TaskManager::addTaskRepeating([this]
  {
    exe->Forward(true);

    /*
     * Display Graph
     */
    if (iteration% (1000) == 0)
    {
      //cout << "Y: " << exe->outputs[0] << exe->outputs[0].At(0,0) << endl;

      MSE mse;
      RMSE rmse;
      // Forward pass is enough as no gradient is needed when evaluating
      exe->Forward(false);

      // reshape label to 1d
      auto labelData = graphValues["label"].Reshape( Shape(graphValues["label"].GetShape()[0]) );

      vector<float> labelDataLocal;
      vector<float> ys;
      exe->outputs[0].SyncCopyToCPU(&ys, exe->outputs[0].Size());
      graphValues["label"].SyncCopyToCPU(&labelDataLocal, graphValues["label"].Size());

      debug("label: " + Json{labelDataLocal}.dump());
      debug("real:  " + Json{ys}.dump());

      mse.Update(labelData, exe->outputs[0]);
      rmse.Update(labelData, exe->outputs[0]);

      chartNetworkProgress.addDataPoint(0, {iteration}, rmse.Get());
      chartNetworkProgress.addDataPoint(1, {iteration}, mse.Get());

      //info("mse:  " + to_string(mse.Get()));
      //info("rmse: " + to_string(rmse.Get()));

      chartNetworkOutput.recalcData();

      // set old data
      graphValues["x"].SyncCopyFromCPU(trainDataX);
    }


    /*
     * Backprop
     */
    exe->Backward();

    // Update parameters
    for (size_t i = 0; i < this->graphValueNames.size(); ++i)
    {
      if (this->graphValueNames[i] == "x" || this->graphValueNames[i] == "label") continue;
      optimizer->Update(i, exe->arg_arrays[i], exe->grad_arrays[i]);
    }

    iteration++;
  });


  cout << endl;
  //printMap(graphValues);
}

void NeuralNetwork::trainInNewThread()
{
  this->trainThread = new thread(&NeuralNetwork::train, this);
}

void NeuralNetwork::joinTrainThread()
{
  trainThread->join();
}


void NeuralNetwork::stopTrain()
{
  this->trainEnable = false;
}



// -- HELPER ---------------------------------------------
void NeuralNetwork::printSymbolShapes(map<string, NDArray> map1)
{
  stringstream out;
  for (auto& t : map1)
  {
    out << t.first << ": (";

    for (auto s : t.second.GetShape())
      out << s << ",";

    out << ")   "; //<< t.second << "";
  }
  info("symbol shapes: " + out.str());
};



NeuralNetwork::~NeuralNetwork()
{
  //stopTrain();
  info("destruct NeuralNetwork");
}
void NeuralNetwork::shutdown()
{
  // exit
  MXNotifyShutdown();
}