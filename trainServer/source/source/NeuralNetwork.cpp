#include <Frontend.h>
#include "NeuralNetwork.h"

Context *NeuralNetwork::ctx;


void NeuralNetwork::init(Context *mxContext)
{
  ctx = mxContext;
}


// constructor
NeuralNetwork::NeuralNetwork(DataStructure *structure, Json params)
    : NeuralNetwork(structure)
{
  fromJson(params);
}

NeuralNetwork::NeuralNetwork(DataStructure *structure)
    : JsonListItem(structure->networkIdIncrement), structure(*structure)
{
  // clear display
  auto chartFunc = Frontend::getChart("progress");
  chartFunc.setGraphData("error",  {}, {}).changeApply();

  auto numHidden = 40;
  batchSize = 20;

  /* --------------------------------------
   * Graph
   */
  auto symX       = Symbol::Variable("x");
  auto symLabel   = Symbol::Variable("label");


  auto symL1 = tanh( FullyConnected(
      "fullyConnected",
      symX,
      Symbol::Variable("weight1"),
      Symbol::Variable("bias1"),
      numHidden   /* Num hidden */
  ));

  auto symL2 = tanh( FullyConnected(
      "fullyConnected",
      symL1,
      Symbol::Variable("weight2"),
      Symbol::Variable("bias2"),
      numHidden   /* Num hidden */
  ));

  auto symL3 = tanh( FullyConnected(
      "fullyConnected",
      symL2,
      Symbol::Variable("weight3"),
      Symbol::Variable("bias3"),
      numHidden   /* Num hidden */
  ));


  auto symOut = tanh( FullyConnected(
      "fullyConnected",
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
  cout << "new net created" << endl;
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

  // load training data
  /*
  NDArray({
              1,2,3,4,5,6,7,8,9,10
          }, Shape(graphValues["x"].GetShape()), ctx).CopyTo(&graphValues["x"]);

  NDArray({
              5,6,7,8,7,6,5,2,0,-2
          }, Shape(graphValues["label"].GetShape()), ctx).CopyTo(&graphValues["label"]);
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
  auto chartFunc = Frontend::getChart("outputShape");
  vector<float> xs;
  vector<float> ys;
  graphValues["label"].SyncCopyToCPU(&ys, batchSize);
  graphValues["x"].SyncCopyToCPU(&xs, batchSize);
  chartFunc.setGraphData("real",  xs, ys).changeApply();



  // train loop
  cout << endl << "-- Train ----------------" << endl;
  trainEnable = true;


  while (trainEnable)
  {
    exe->Forward(true);

    if (iteration% (1000) == 0)
    {
      cout << "Y: " << exe->outputs[0] << exe->outputs[0].At(0,0) << endl;

      auto chart = Frontend::getChart("progress");
      chart.setGraphData("error", {iteration}, {exe->outputs[0].At(0,0)}).addApply();

      auto chartFunc = Frontend::getChart("outputShape");
      vector<float> xs;
      vector<float> ys;
      exe->outputs[0].SyncCopyToCPU(&ys, batchSize);
      graphValues["x"].SyncCopyToCPU(&xs, batchSize);
      chartFunc.setGraphData("network",  xs, ys).changeApply();
    }

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
  for (auto& t : map1)
  {
    std::cout << t.first << ": (";

    for (auto s : t.second.GetShape())
      cout << s << ",";

    cout << ")   "; //<< t.second << "";

    cout << endl;
  }
};



// -- JSON -------------------------------------------------
Json NeuralNetwork::toJson()
{
  return JsonListItem::toJson().merge(Json {
      {"name", name},
      {"hidden", hiddenLayers},
      {"neuronsPerHidden", neuronsPerLayer}
      //{"learnRate", learnrate},
      //{"optimizer", optimizer},
  });
}
void NeuralNetwork::fromJson(Json params)
{
  JsonListItem::fromJson(params);
  name = params["name"];
  hiddenLayers = params["hidden"];
  neuronsPerLayer = params["neuronsPerHidden"];
  //learnrate = params["learnRate"];
  //optimizer = params["optimizer"];
}


NeuralNetwork::~NeuralNetwork()
{
  //stopTrain();
}
void NeuralNetwork::shutdown()
{
  // exit
  MXNotifyShutdown();
}
