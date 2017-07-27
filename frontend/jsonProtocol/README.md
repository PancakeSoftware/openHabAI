# Backend ~> Frontend
## Update Charts
```
{
  "type": "updateChart",
  "what":{
      "chart": "progress",
      "type": "change",
      "graphs": [
        {
          "graph": "error",
          "data": [
            {
              "x": 1,
              "y": 8
            }
          ]
        }
      ]
    }
}
```


# Frontend ~> Backend
## Do
```
{
  "type": "do",
  "what": "startTrain",
  "data": {
    "datastructure": 2,
    "network": 2
  },
  "respondId": 12
}
```
respond:
```
{
  "type": "respond",
  "what": "ok",
  "respondId": 12
}
```


## DataStructs
#### Get
```
{
  "type": "get",
  "what": "datastructures",
  "respondId": 12
}
```
respond:
```
{
  "type": "respond",
  "respondId": 12,
  "what": [
    {
        "name": "first",
        "type": "function",
        "id": 1
    }
  ]
}
```
#### Create
```
{
  "type": "create",
  "what": "datastructure",
  "respondId": 12,
  "data": {
    "name": "Test",
    "type": "function",
    "function": "x^2 + sin(x)",
    "range": {
        "from": -10,
        "to": 10
    }
  }
}
```
respond:
```
{
  "type": "respond",
  "respondId": 12,
  "what": "ok",
  "data": {
    "dataStructureId": 2,
    "name": "Test",
    "type": "function"
  }
}
```



## Networks
#### Get
```
{
  "type": "get",
  "what": "networks",
  "data": {
    "datastructure": 2
  },
  "respondId": 12
}
```
respond:
```
{
  "type": "respond",
  "respondId": 12,
  "what": [
    {
        "name": "first",
        "id": 1
    }
  ]
}
```
#### Create
```
{
  "type": "create",
  "what": "network",
  "respondId": 12,
  "data": {
    "datastructure": 2,
    "name": "Test Net",
    "hidden": 2,
    "neuronsPerHidden": 2
  }
}
```
respond:
```
{
  "type": "respond",
  "respondId": 12,
  "what": "ok",
  "data": {
    "networkId": 2,
    "name": "Test Net"
  }
}
```


## Network
#### Get
```
{
  "type": "get",
  "what": "network",
  "respondId": 12,
  "data": {
     "datastructure": 2,
     "networkId": 2
  }
}
```
respond:
```
{
  "type": "respond",
  "respondId": 12,
  "what": {
    "name": "Test Net",
    "learnRate": 0.001
  }
}
```