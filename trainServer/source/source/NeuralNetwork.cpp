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
  chartNetworkOutput.setInputOutputNames(structure->inputNames, structure->outputNames);
  chartNetworkOutput.setUpdateFunctionRanged([this, structure] (const vector<RangeParam> &inputRanges, vector<ValueParam> fixedInputs, const vector<int> &outputIds) {
    Executor* exeChart = symLossOut.SimpleBind(*ctx, graphValues, map<std::string, NDArray>());
    map<int, vector<ChartDataPoint>> result;

    // generate input data
    auto dataX =  createInputDataGrid(inputRanges, fixedInputs);
    vector<float> dataXFlatten = flatten2DVec(dataX);
    //debug("inputValues: " + Json{dataX}.dump());
    int inputDimensions = structure->inputNames.size();
    int iterations = ceil((float)dataX.size() / (float)batchSize);

    // iterate throw whole data
    for (int i = 0; i < iterations; ++i) {
      // push throw model
      graphValues["x"].SyncCopyFromCPU(dataXFlatten.data() + i*inputDimensions*batchSize, batchSize*inputDimensions); // @TODO in last iteration data is smaller than batchsize -> Error!?
      exeChart->Forward(false);

      // sync back
      vector<float> ys;
      exeChart->outputs[0].SyncCopyToCPU(&ys, batchSize);

      // add to result
      for (int j = 0; (j < ys.size()) && (i*batchSize+j < dataX.size()); ++j) {
        result[0].push_back(ChartDataPoint(ys[j], dataX[i*batchSize+j]));
        //debug("i: " + to_string(i) + "  j: " + to_string(j)+ "  val: "+ to_string(ys[j]) +"   dataX[i*batchSize+j]: " + Json{dataX[i*batchSize+j]}.dump());
      }
    }

    // set old data
    graphValues["x"].SyncCopyFromCPU(trainDataX);

    delete exeChart;
    return result;
  });
  chartNetworkProgress.setInputOutputNames({"iteration"}, {"rmse-loss", "mse-loss"});



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
      {"x", NDArray(Shape(batchSize /* batch */, structure.inputNames.size() /*inputs*/), *ctx)},
      {"label", NDArray(Shape(batchSize, structure.outputNames.size()/* outputs */), *ctx)}
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
  for (auto record : data)
  {
    for (float inDat : record.first)
      trainDataX.push_back(inDat); // push inputValues after each other

    // set label data
    for (float outDat : record.second)
      y.push_back(outDat);
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
  printSymbolShapes(graphValues);


  // display function
  vector<float> xs;
  vector<float> ys;
  graphValues["label"].SyncCopyToCPU(&ys, batchSize);
  graphValues["x"].SyncCopyToCPU(&xs, batchSize);
  //debug("x: " + Json{xs}.dump());
  //debug("label: " + Json{ys}.dump());


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

      //debug("label: " + Json{labelDataLocal}.dump());
      //debug("real:  " + Json{ys}.dump());

      mse.Update(labelData, exe->outputs[0]);
      rmse.Update(labelData, exe->outputs[0]);

      chartNetworkProgress.addDataPoint(0, {iteration}, rmse.Get());
      chartNetworkProgress.addDataPoint(1, {iteration}, mse.Get());

      chartNetworkOutput.recalcData();

      // set old data
      // graphValues["x"].SyncCopyFromCPU(trainDataX);
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