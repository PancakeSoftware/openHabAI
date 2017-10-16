
# OpenHabAI<br>![](frontend/img/frontend.png)
Automate your home using Neuronal networks. <br>
OpenHabAI provides a fast c++ backend 
([mxnet is used for computation](http://mxnet.io)) and frontend that run in browser.


## Build [![](https://travis-ci.org/JeyRunner/openHabAI.svg?branch=master)](https://travis-ci.org/JeyRunner/openHabAI)
First **install** these packages:
* For **frontend**
    * nodejs
    * npm
* For **trainServer**
    * zlib1g-dev
    * for **mxnet**
        * libopenblas-dev 
        * liblapack-dev
        * *cuda (optional, if you want to use gpu)* [see at mxnet.io](http://mxnet.io/get_started/build_from_source.html#optional-cuda-cudnn-for-nvidia-gpus)
    * cmake, git, c++ build tools
    
Execute build command:
```bash
mkdir build
cd build
cmake ../
make 
```
The compiled trainSever executable can be found in build/bin. <br>
To run frontend: ```make frontendRun``` or see in [README of frontend](frontend/README.md)

 
#### Protocol
See backend-frontend [protocol definition](./frontend/jsonProtocol/README.md).
