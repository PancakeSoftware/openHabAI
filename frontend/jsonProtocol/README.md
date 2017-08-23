## Basic packet structure
There are two main types of packages:
1.  ##### Request
    ```json
    {
      "type": "request",
      "route": [
        {"Component-A":   "entityID-A"},
        {"Sub-Component": "entityID-B"},
      ],
      "what": "add|remove|get|getAll",
      "data": {
        
      },
      "respondId": 12 
    }
    ```
    *   **route** defines something similar to rest url, in this example you want to access:<br>
        ```entityID-A of Component-A / entityID-B of Sub-Component```<br>
        Of every route object, key corresponds to component and value corresponds to entity of component.
        If entity is empty you want ot access component as whole.
    *   **what** defines action to perform, allowed values depend on route.
    *   **data** contains data that is necessary to perform action. 
    *   **respondId** `(optional)` if defined package requires respond, the responds *respondId* field will have same value as here specified. 
2.  ##### Respond
    ```json
    {
      "type": "respond",
      "what": "ok|error|add|remove|get|getAll",
      "data": {
        
      },
      "respondId": 12
    }
    ```
    *   **respondId**  has same value as in request package.
    
If type key not exists, package is treated as request.



# Backend ~> Frontend
## Update Charts
```json
{
  "type": "updateChart",
  "what": "",
  "data": {
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
Register chart updates for network (for unsubscribe:  ```"type": "unsubscribe"```)
```json
{
  "type": "subscribe",
  "what": "updateChart",
  "data": {
    "datastructure": 2,
    "network": 2
  }
}
```


# Frontend ~> Backend
## Do
#### Start train
```json
{
  "type": "do",
  "what": "startTrain",
  "data": {
    "datastructure": 2,
    "network": 2,
    "optimizer": "sgd",
    "learnRate": 0.01
  },
  "respondId": 12
}
```
respond:
```json
{
  "type": "respond",
  "what": "ok",
  "respondId": 12
}
```


## DataStructs
#### Get
type:"getAll" - get all elements<br>
type:"get" - get one element (define id in "data")
```json
{
  "type": "getAll",
  "what": "datastructure",
  "respondId": 12
}
```
respond:
```json
{
  "type": "respond",
  "respondId": 12,
  "what": "ok",
  "data": [
    {
        "name": "first",
        "type": "function",
        "id": 1
    }
  ]
}
```
#### Create
```json
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
```json
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
type:"getAll" - get all elements<br>
type:"get" - get one element (define id in "data")
```json
{
  "type": "getAll",
  "what": "networks",
  "data": {
    "datastructure": 2
  },
  "respondId": 12
}
```
respond:
```json
{
  "type": "respond",
  "respondId": 12,
  "what": "ok",
  "data":[
    {
        "name": "first",
        "id": 1
    }
  ]
}
```
#### Create
```json
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
```json
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
```json
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
```json
{
  "type": "respond",
  "respondId": 12,
  "what": "ok",
  "data": {
    "name": "Test Net",
    "learnRate": 0.001,
    "optimizer": "sgd"
  }
}
```