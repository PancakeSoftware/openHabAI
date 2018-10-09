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
    auto initializer = Uniform(initUniformDistributionScale);
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
      graphValues["Input"].SyncCopyFromCPU(dataXFlatten.data() + i*inputDimensions*batchSize, batchSize*inputDimensions); // @TODO in last iteration data is smaller than batchsize -> Error!?
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
    graphValues["Input"].SyncCopyFromCPU(trainDataX);

    delete exeChart;
    return result;
  });
  chartNetworkProgress.setInputOutputNames({"iteration"}, {"rmse-loss", "mse-loss"});



  /* --------------------------------------
   * Graph
   */
  auto numHidden = 40;
  auto symX       = Symbol::Variable("Input");
  auto symLabel   = Symbol::Variable("Label");


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


void NeuralNetwork::setModelDefinition(vector<OperationNode>  model)
{
  if (model.size() == 0)
    return;

  /*
   * topological sorting of model
   */
  map<int, int> fromToPos;            // old to new node position
  vector<OperationNode>  modelCopy = model;   // work with model copy
  vector<OperationNode>  modelSorted; // sorted model

  // set index of each node
  int index = 0;
  for (auto it = modelCopy.begin(); it != modelCopy.end(); it++) {
    it->index = index;
    index++;
  }

  // sort and change inputs
  while (modelCopy.size() > 0)
  {
    bool hasCircle = true;

    // find nodes with no input -> remove
    for (auto it = modelCopy.begin(); it != modelCopy.end();)
    {
      if (it->inputNodes.size() == 0) {
        int pos = it->index;
        OperationNode done = model[pos];

        // change input nodes to new indices
        for (auto inputIt = done.inputNodes.begin(); inputIt != done.inputNodes.end(); inputIt++) {
          *inputIt = fromToPos[*inputIt];
        }
        modelSorted.push_back(done);
        fromToPos.emplace(pos, modelSorted.size()-1);
        it = modelCopy.erase(it);
        // erase index of removed node form all inputs
        for (auto nodeIt = modelCopy.begin(); nodeIt != modelCopy.end(); nodeIt++) {
          for (auto inputIt = nodeIt->inputNodes.begin(); inputIt != nodeIt->inputNodes.end();) {
            if (*inputIt == pos)
              inputIt = nodeIt->inputNodes.erase(inputIt);
            else
              inputIt++;
          }
        }
        hasCircle = false;
      }
      else
        it++;
    }

    // circle is in graph
    if (hasCircle)
      throw JsonObjectException("circle in model graph");
  }

  // set sorted model
  // debug("new model: " + Json{modelSorted}.dump());
  modelDefinition = modelSorted;
  //notifyParamsChanged({"modelDefinition"});


  /*
   * create mxnet graph from model
   */
  // input, label symbols
  Symbol symInput;
  Symbol symLabel;
  Symbol symLoss;
  vector<Symbol> mxModel(0);

  index = -1;
  for (OperationNode node: modelDefinition)
  {
    index++;

    // if is input, label
    if (node.operation == "Input") {
      Symbol sym = Symbol::Variable(node.operation);
      mxModel.push_back(sym);
      symInput = sym;
      continue;
    }
    if (node.operation == "Label") {
      Symbol sym = Symbol::Variable(node.operation);
      mxModel.push_back(sym);
      symLabel = sym;

      Symbol input;
      if (node.inputNodes.size() >= 1)
        input = mxModel[node.inputNodes[0]];
      else
        input = Symbol("");
      symLoss =  Operator(node.opParams["lossFunction"].get<string>())
          .SetParam("grad_scale", 1)
          .SetInput("data", input)
          .SetInput("label", symLabel)
          .CreateSymbol(node.opParams["lossFunction"]);
      continue;
    }


    Symbol sym;

    if (node.operation == "FullyConnected") {
      Symbol input;
      // input symbols
      if (node.inputNodes.size() == 1)
        input = mxModel[node.inputNodes[0]];
      else if (node.inputNodes.size() > 1) {
        input = mxModel[node.inputNodes[0]];
        warn("operations with multiple inputs not supported yet (will use first input only), at " + routeString);
      }
      else {
        input = Symbol("");
        warn("operation has no input (antonym input is created), at " + routeString);
      }

      sym = FullyConnected(
          node.name,
          input,
          Symbol::Variable(to_string(index) + "-weight"),
          Symbol::Variable(to_string(index) + "-bias2"),
          stoi(node.opParams["num_hidden"].get<string>())
      );

      if (node.opParams["activationFunction"] != "none") {
        sym = Operator(node.opParams["activationFunction"].get<string>())
            .SetInput("data", sym)
            .CreateSymbol();
      }
    }

    mxModel.push_back(sym);

    /*
    // create op and set params
    Operator op(node.operation);

    for (Json::iterator it = node.opParams.begin(); it != node.opParams.end(); ++it) {
      if (it.key() == "activationFunction")
        continue;
      debug("set param: " + it.key() + ": " + (it.value().dump()));
      op.SetParam(it.key(), it.value());
    }

    // input symbols
    if (node.inputNodes.size() == 1)
      op.SetInput("data", mxModel[node.inputNodes[0]]);
    else if (node.inputNodes.size() > 1) {
      op.SetInput("data", mxModel[node.inputNodes[0]]);
      warn("operations with multiple inputs not supported yet (will use first input only), at " + routeString);
    }

    // operation specific inputs/params
    if (node.operation == "FullyConnected") {
      op.SetInput("weight", Symbol::Variable(to_string(index) + "-weight"))
        .SetInput("bias",   Symbol::Variable(to_string(index) + "-bias"))
        .SetParam("no_bias", false).SetParam("num_hidden", 100)
        .SetParam("flatten", true);
      if (node.opParams["num_hidden"] <= 0)
        continue;
    }

    //Symbol sym = op.CreateSymbol(node.name);
    Operator opT("FullyConnected");
    opT .SetParam("num_hidden", 100)
        .SetParam("no_bias", false)
        .SetParam("flatten", true)
        .SetInput("data", mxModel[node.inputNodes[0]])
        .SetInput("weight", Symbol::Variable(to_string(index) + "weight"))
        .SetInput("bias", Symbol::Variable(to_string(index) + "bias"));
    Symbol sym = op.CreateSymbol(node.name);//opT.CreateSymbol(node.name);//*-/

    debug("op symbol: " + sym.ToJSON());
    mxModel.push_back(sym);
     */
  }

  if (symInput.GetHandle() == nullptr || symLabel.GetHandle()  == nullptr)
    throw JsonObjectException("model definition not has Input or Label");



  symLossOut = symLoss;
  //debug(symLoss.ToJSON());
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
      {"Input", NDArray(Shape(batchSize /* batch */, structure.inputNames.size() /*inputs*/), *ctx)},
      {"Label", NDArray(Shape(batchSize, structure.outputNames.size()/* outputs */), *ctx)}
  };


  // inter args
  try
  {
    symLossOut.InferArgsMap(*ctx, &graphValues, graphValues);
    graphValueNames = symLossOut.ListArguments();
  }
  catch (exception &e) {
    err(e.what());
    cout << "Last err:" << MXGetLastError() << endl;
    throw JsonObjectException("InferArgsMap of model graph: " + string(MXGetLastError()));
  }


  // -- INIT ALL  ----
  auto initializer = Uniform(initUniformDistributionScale);
  for (auto& arg : graphValues) {
    // arg.first is parameter name, and arg.second is the value
    initializer(arg.first, &arg.second);
    //arg.second = 1;
  }

  printSymbolShapes(graphValues);

  try {
    exe = symLossOut.SimpleBind(*ctx, graphValues, map<std::string, NDArray>()); //, graphGradientOps);
  }
  catch (exception &e) {
    err("bind executor to model-graph", e.what());
    err("bind executor to model-graph", string(MXGetLastError()));
    throw JsonObjectException("bind executor to model-graph: " + string(MXGetLastError()));
  }
}


// -- TRAIN -----------------------------------------
void NeuralNetwork::train()
{
  // save to file
  if (route.is_initialized()) {
    string path = route.get().toStringStorePath() + "model.json";
    ofstream stream;
    stream.open(path);
    if (!stream.is_open()) {
      stream.close();
      err("save to " + path);
    } else
      stream << symLossOut.ToJSON();
    stream.close();
  }


  /* --------------------------------------
  * Train
  */
  if (exe != nullptr)
    delete exe;
  if (optimizer != nullptr)
    delete optimizer;

  try {
    exe = symLossOut.SimpleBind(*ctx, graphValues, map<std::string, NDArray>()); //, graphGradientOps);
  }
  catch (exception &e) {
    cout << e.what() << endl;
    cout << "Last err:" << MXGetLastError() << endl;
  }




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
  graphValues["Input"].SyncCopyFromCPU(trainDataX);
  graphValues["Label"].SyncCopyFromCPU(y);

  cout << endl;
  cout << "X("<< graphValues["Input"].Size() <<"): " << graphValues["Input"] << endl;
  cout << "L("<< graphValues["Label"].Size() <<"): " << graphValues["Label"] << endl;
  printSymbolShapes(graphValues);


  // display function
  vector<float> xs;
  vector<float> ys;
  graphValues["Label"].SyncCopyToCPU(&ys, batchSize);
  graphValues["Input"].SyncCopyToCPU(&xs, batchSize);
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
      auto labelData = graphValues["Label"].Reshape( Shape(graphValues["Label"].GetShape()[0]) );

      vector<float> labelDataLocal;
      vector<float> ys;
      exe->outputs[0].SyncCopyToCPU(&ys, exe->outputs[0].Size());
      graphValues["Label"].SyncCopyToCPU(&labelDataLocal, graphValues["Label"].Size());

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
      if (this->graphValueNames[i] == "Input" || this->graphValueNames[i] == "Label") continue;
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