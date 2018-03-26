#include <Frontend.h>
#include <util/TaskManager.h>
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
      chartProgressT(),
      charts(),
      structure(*structure),
      chartProgress(Frontend::getChart("progress")),
      chartShape(Frontend::getChart("outputShape"))
{
  setLogName("NETWORK");

  /*
   * mount charts */
  setSubRoutes({{"charts", &charts}});
  charts.setSubRoutes({{"progress", &chartProgressT}});

  /*
   * charts update functions */
  chartProgressT.setUpdateFunction([this] (map<int, float> &inputValues, vector<int> &outputIds) {
    map<int, float> out;
    for (int id: outputIds)
      out[id] = inputValues[0]+inputValues[1]+inputValues[2]+inputValues[3] * id;
    return out;
  });


  // clear display
  chartProgress.setGraphData("error",  {}, {}).changeApply();

  auto numHidden = 40;
  batchSize = 20;

  /* --------------------------------------
   * Graph
   */
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



  /* --------------------------------------
   * Graph Value Arrays
   */

  graphValues = {
      {"x", NDArray(Shape(batchSize, 1), *ctx)},
      {"label", NDArray(Shape(batchSize, 1), *ctx)}
  };


  // inter args
  symLossOut.InferArgsMap(*ctx, &graphValues, graphValues);
  this->graphValueNames = symLossOut.ListArguments();


  // -- INIT ALL  ----
  auto initializer = Uniform(0.01);
  for (auto& arg : graphValues) {
    // arg.first is parameter name, and arg.second is the value
    initializer(arg.first, &arg.second);
    //arg.second = 1;
  }

  printSymbolShapes(graphValues);
  ok("new net created");
}



// -- TRAIN -----------------------------------------
void NeuralNetwork::train()
{
  /* --------------------------------------
  * Train
  */
  Executor* exe = symLossOut.SimpleBind(*ctx, graphValues, map<std::string, NDArray>()); //, graphGradientOps);

  // setup optimizer
  Optimizer *optimizer = OptimizerRegistry::Find("sgd");
  optimizer
      ->SetParam("rescale_grad", 1.0)
      ->SetParam("lr", 0.005)           // learn rate
      ->SetParam("wd", 0.01);           // weight decay



  /* -----------------------------------
   * Generate Train data
   */
  vector<float> x;
  vector<float> y;

  for (float i = 0; i < batchSize/2; i+= 0.5)
  {
    x.push_back(i);
    y.push_back(sin(i*1)*pow(i+2, 2)*0.012);
  }

  graphValues["x"].SyncCopyFromCPU(x);
  graphValues["label"].SyncCopyFromCPU(y);

  cout << endl;
  cout << "X: " << graphValues["x"] << endl;
  cout << "L: " << graphValues["label"] << endl;


  // display function
  vector<float> xs;
  vector<float> ys;
  graphValues["label"].SyncCopyToCPU(&ys, batchSize);
  graphValues["x"].SyncCopyToCPU(&xs, batchSize);
  chartShape.setGraphData("real",  xs, ys).changeApply();


  //vector<float> xs{1,2,3};
  //chartShape.setGraphData("real",  xs, structure.getDataBatch(xs)).changeApply();


  /* --------------------
   * train loop
   */
  cout << endl << "-- Train ----------------" << endl;
  trainEnable = true;


  while (trainEnable)
  {
    exe->Forward(true);

    /*
     * Display Graph
     */
    if (iteration% (1000) == 0)
    {
      cout << "Y: " << exe->outputs[0] << exe->outputs[0].At(0,0) << endl;

      chartProgress.setGraphData("error", {iteration}, {exe->outputs[0].At(0,0)}).addApply();

      vector<float> xs;
      vector<float> ys;
      exe->outputs[0].SyncCopyToCPU(&ys, batchSize);
      graphValues["x"].SyncCopyToCPU(&xs, batchSize);
      chartShape.setGraphData("network",  xs, ys).changeApply();
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
  }


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
}
void NeuralNetwork::shutdown()
{
  // exit
  MXNotifyShutdown();
}

ApiRespond *NeuralNetwork::processApi(ApiRequest request)
{
  ApiRespond* respond = ApiRouteJson::processApi(request);
  if (respond != nullptr || request.route.size() > 0)
    return respond;

  /*
   * Networks Api */
  if (request.what == "do") {
    if (request.data["do"] == "startTrain") {
      TaskManager::addTaskOnceOnly([this]{ info("train from task ..."); });
      chartProgressT.pushUpdate();
      //return new ApiRespondError("not supported", request);
    }
    else if (request.data["do"] == "stopTrain") {
      return new ApiRespondError("not supported", request);
    }
  }
}
