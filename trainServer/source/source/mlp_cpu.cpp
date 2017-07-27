
#include <iostream>
#include <vector>
#include <string>
#include "mxnet-cpp/MxNetCpp.h"
#include "mxnet/c_api.h"
// Allow IDE to parse the types


using namespace std;
using namespace mxnet::cpp;


void printMap(map<string, NDArray> map1)
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

void printMapVec(map<string, vector<NDArray>> map1)
{
  for (auto& t : map1)
  {
    std::cout << t.first << ":";
    for (auto item : t.second)
      cout << item << " ";
  }
  cout << endl;
};

template <typename T>
void printVector(vector<T> v)
{
  for (auto item : v)
    cout << item << " ";
  cout << endl;
}


int batch_size = 10;

int train()
{
  // -- Net
  auto input = Symbol::Variable("x");
  auto bias = Symbol::Variable("bias");
  auto label = Symbol::Variable("label");

  //auto l1 = FullyConnected(varA, weights, bias, 1);

  auto y = input + bias;//add_n({input, bias});//Activation(l1, ActivationActType::kRelu);
  //auto lossOutput = LinearRegressionOutput("loss", y, label);
  auto lossOutput = LinearRegressionOutput(y, label);//MakeLoss("loss", l);


  // == run ======================================
  Context ctx(DeviceType::kCPU, 0);


  std::map<string, NDArray> args;
  args["x"] = NDArray(Shape(1), ctx);
  args["label"] = NDArray(Shape(1), ctx);


  // exec


  std::map<string, NDArray> argsP;

  // autodetect inputs -/- weights
  lossOutput.InferArgsMap(ctx, &argsP, args);

  cout << "---------" << endl;
  printMap(argsP);
  printMap(args);
  cout << "---------" << endl;

  // init ?????
  auto initializer = Uniform(0.01);
  for (auto& arg : argsP) {
    // arg.first is parameter name, and arg.second is the value
    //initializer(arg.first, &arg.second);
  }

  argsP["bias"] = 1;


  // Create executor by binding parameters to the model

  //auto *exec = lossOutput.SimpleBind(ctx, argsP);
  //auto *execY = y.SimpleBind(ctx, argsP);



  // -- exec ------------------------------------
  auto biasArr = NDArray(Shape(1,2), ctx);
  auto xArr = NDArray(Shape(1), ctx);
  auto labelArr = NDArray(Shape(1), ctx);
  auto biasArrGrad = NDArray(Shape(1,2), ctx);
  vector<NDArray> arg_grad_store = {
      NDArray(),
      biasArrGrad,
      NDArray()
  };

  vector<NDArray> in_args = {
      xArr,
      biasArr,
      labelArr
  };

  vector<OpReqType> grad_req_type = {
      kNullOp,
      kWriteTo,
      kNullOp
  };

  biasArr = 0;
  vector<NDArray> aux_states;



  cout << "make the Executor" << endl;
  Executor* exec = new Executor(lossOutput, ctx, in_args, arg_grad_store,
                               grad_req_type, aux_states);

  Executor* execY = new Executor(y, ctx, in_args, arg_grad_store,
                                grad_req_type, aux_states);
  // -----------------------------------------------------




  // optimize
  Optimizer *optimizer = OptimizerRegistry::Find("sgd");


  auto data_x = NDArray({1, 1}, Shape(1), ctx);
  auto data_y = NDArray({5, -2}, Shape(1), ctx);

  for (int i = 0; i < 100; ++i)
  {
    // copy data to graph
    data_x.CopyTo(&xArr);//&argsP["x"]);
    data_y.CopyTo(&labelArr);//argsP["label"]);

    // CopyTo is imperative, need to wait for it to complete.
    NDArray::WaitAll();

    //printMap(argsP);
    printVector(in_args);

    // calculate
    exec->Forward(true);
    cout <<  "=> L: " << exec->outputs[0] << endl;

    exec->Backward();
    exec->UpdateAll(optimizer, 0.1, 0.1);



    // show y

    // CopyTo is imperative, need to wait for it to complete.
    NDArray::WaitAll();

    execY->Forward(false);

    cout <<  "=> Y: " << execY->outputs.data()[0] << endl << endl;



    /*
    Accuracy acc;
    data_x.CopyTo(&args["x"]);
    data_y.CopyTo(&args["label"]);

    auto *exec = lossOutput.SimpleBind(ctx, args);
    // Forward pass is enough as no gradient is needed when evaluating
    exec->Forward(false);
    acc.Update(NDArray({2, 1}, Shape(2), ctx), exec->outputs[0]);
    cout << "ACC: " << acc.Get() << endl;
    delete exec;
     */
  }


  MXNotifyShutdown();
  return 0;
}


void trainNew()
{
  /* --------------------------------------
   * CPU context
   */
  Context ctx(DeviceType::kCPU, 0);


  auto numHidden = 10;
  auto batchSize = 7;

  /* --------------------------------------
   * Graph
   */
  auto symX       = Symbol::Variable("x");
  auto symLabel   = Symbol::Variable("label");

  auto symWeight1  = Symbol::Variable("weight1");
  auto symBias1    = Symbol::Variable("bias1");
  auto symWeight2  = Symbol::Variable("weight2");
  auto symBias2    = Symbol::Variable("bias2");


  auto symL1 = LeakyReLU( FullyConnected(
      "fullyConnected",
      symX,
      symWeight1,
      symBias1,
      numHidden   /* Num hidden */
  ));

  auto symOut = LeakyReLU( FullyConnected(
      "fullyConnected",
      symL1,
      symWeight2,
      symBias2,
      1   /* Num hidden */
  ));

  //auto symOut = dot("multi", symX,  symWeight1);

  auto symLossOut = LinearRegressionOutput(
      "linearRegression",
      symOut,
      symLabel
  );



  /* --------------------------------------
   * Graph Value Arrays
   */
  auto arrayX       = NDArray(Shape(batchSize, 1), ctx);
  auto arrayLable   = NDArray(Shape(batchSize, 1), ctx);

  auto arrayWeight1  = NDArray(Shape(numHidden, 1), ctx);
  auto arrayBias1    = NDArray(Shape(numHidden), ctx);
  auto arrayWeight2  = NDArray(Shape(1, numHidden), ctx);
  auto arrayBias2    = NDArray(Shape(1), ctx);

  vector<NDArray> in_args = {
      arrayX,
      arrayWeight1,
      arrayBias1,
      arrayWeight2,
      arrayBias2,
      arrayLable
  };

  vector<NDArray> arg_grad_store = {
      NDArray(), // x
      NDArray(arrayWeight1.GetShape(), ctx),
      NDArray(arrayBias1.GetShape(), ctx),
      NDArray(arrayWeight2.GetShape(), ctx),
      NDArray(arrayBias2.GetShape(), ctx),
      NDArray()  // label
  };

  vector<OpReqType> grad_req_type = {
      kNullOp,
      kWriteTo,
      kWriteTo,
      kWriteTo,
      kWriteTo,
      kNullOp
  };


  Executor* exe = new Executor(
      symLossOut,
      ctx,
      in_args,
      arg_grad_store,
      grad_req_type,
      *(new vector<NDArray>())
  );



  /* --------------------------------------
   * Train
   */
  Optimizer *optimizer = OptimizerRegistry::Find("sgd");

  // init
  arrayBias1 = 0;
  arrayBias2 = 0;
  arrayWeight1 = 0.01;
  arrayWeight2 = 0.01;

  // load training data
  NDArray({
      1,2,3,4,5,6,7
  }, Shape(arrayX.GetShape()), ctx).CopyTo(&arrayX);

  NDArray({
      5,6,7,8,7,6,5
  }, Shape(arrayLable.GetShape()), ctx).CopyTo(&arrayLable);

  cout << "X: " << arrayX << endl;
  cout << "L: " << arrayLable << endl;

  // train loop

  // show test output
  exe->Forward(false);
  cout << exe->outputs[0] << endl;

  cout << endl << "-- Train ----------------" << endl;

  for (int i = 0; i < 10000; ++i)
  {
    exe->Forward(true);

    if (i%100 == 0)
    {
      Accuracy acu;
      //acu.Update(arrayLable, exe->outputs[0]);
      cout << "acc: " << acu.Get() << "   Y: " << exe->outputs[0] << endl;
    }

    exe->Backward();
    //cout << "G: " << arg_grad_store[4] << endl;

    exe->UpdateAll(optimizer, 0.001, 0.01);

  }


  // exit
  MXNotifyShutdown();
}


void trainSimple()
{
  /* --------------------------------------
   * CPU context
   */
  Context ctx(DeviceType::kCPU, 0);


  auto numHidden = 30;
  auto batchSize = 10;

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
      symL1,
      Symbol::Variable("weight3"),
      Symbol::Variable("bias3"),
      numHidden   /* Num hidden */
  ));

  auto symOut = LeakyReLU( FullyConnected(
      "fullyConnected",
      symL3,
      Symbol::Variable("weightLast"),
      Symbol::Variable("biasLast"),
      1   /* Num hidden */
  ));

  //auto symOut = dot("multi", symX,  symWeight1);

  auto symLossOut = LinearRegressionOutput(
      "linearRegression",
      symOut,
      symLabel
  );



  /* --------------------------------------
   * Graph Value Arrays
   */

  map<std::string, NDArray> graphValues = {
      {"x", NDArray(Shape(batchSize, 1), ctx)},
      {"label", NDArray(Shape(batchSize, 1), ctx)}
  };

  /*
  map<std::string, OpReqType> graphGradientOps = {
      {"x", kNullOp},
      {"weight1", kWriteTo},
      {"bias1", kWriteTo},
      {"weight2", kWriteTo},
      {"bias2", kWriteTo},
      {"label", kNullOp}
  };
   */

  // inter args
  symLossOut.InferArgsMap(ctx, &graphValues, graphValues);

  Executor* exe = symLossOut.SimpleBind(ctx, graphValues, map<std::string, NDArray>()); //, graphGradientOps);



  // -- INIT ALL  ----
  auto initializer = Uniform(0.01);
  for (auto& arg : graphValues) {
    // arg.first is parameter name, and arg.second is the value
    initializer(arg.first, &arg.second);
    //arg.second = 1;
  }

  printMap(graphValues);


  /* --------------------------------------
   * Train
   */
  Optimizer *optimizer = OptimizerRegistry::Find("sgd");


  // load training data
  NDArray({
              1,2,3,4,5,6,7,8,9,10
          }, Shape(graphValues["x"].GetShape()), ctx).CopyTo(&graphValues["x"]);

  NDArray({
              5,6,7,8,7,6,5,2,0,-2
          }, Shape(graphValues["label"].GetShape()), ctx).CopyTo(&graphValues["label"]);

  cout << endl;
  cout << "X: " << graphValues["x"] << endl;
  cout << "L: " << graphValues["label"] << endl;



  // train loop
  cout << endl << "-- Train ----------------" << endl;
  int iterations = 100000;

  for (int i = 0; i < iterations; ++i)
  {
    exe->Forward(true);

    if (i% (iterations/10) == 0)
    {
      cout << "Y: " << exe->outputs[0] << endl;
    }

    exe->Backward();
    exe->UpdateAll(optimizer, 0.001, 0.01);

  }

  cout << endl;
  //printMap(graphValues);

  // exit
  MXNotifyShutdown();
}


int main(int argc, char** argv)
{
  trainSimple();
}
