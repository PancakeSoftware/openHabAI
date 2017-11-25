# Api between backend and frontend
## Basic packet structure
There are two main types of packages:

1.  ##### Request
    ```json
    {
      "type": "request",
      "route": [
        {"Component-A":   "entityID-A"},
        {"Sub-Component": "entityID-B"}
      ],
      "what": "add|update|remove|get|getAll|do|...",
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
      "what": "ok|error|...",
      "data": {
        
      },
      "respondId": 12
    }
    ```
    *   **respondId**  has same value as in request package.
    
If type key not exists, package is treated as request.


## Commands
## DataStructure
#### Get
 * type:"getAll" - get all elements<br>
 * type:"get" - get one element
```json
{
  "type": "request",
  "route": [
    {"datastructure": ""}
  ],
  "what": "getAll",
  "respondId": 12 
}
```
respond:
```json
{
  "type": "respond",
  "what": "ok",
  "data": [
    {
        "name": "first",
        "type": "function",
        "id": 1
    }
  ],
  "respondId": 12,
}
```
#### Create
```json
{
  "type": "request",
  "route": [
    {"datastructure":   ""}
  ],
  "what": "add",
  "data": {
    "name": "Test",
    "type": "function",
    "function": "x^2 + sin(x)",
    "range": {
        "from": -10,
        "to": 10
    }
  },
  "respondId": 12 
}
```
respond:
```json
{
  "type": "respond",
  "what": "ok",
  "data": {
    "id": 2,
    "name": "Test",
    "type": "function"
  },
  "respondId": 12
}
```



## Network
#### Get all
```json
{
  "type": "request",
  "route": [
    {"datastructure": "2"},
    {"network": ""}
  ],
  "what": "getAll",
  "respondId": 12 
}
```
respond:
```json
{
  "type": "respond",
  "what": "ok",
  "data":[
    {
        "name": "first",
        "id": 1
    }
  ],
  "respondId": 12
}
```
#### Create
```json
{
  "type": "request",
  "route": [
    {"datastructure": "2"},
    {"network": ""}
  ],
  "what": "add",
  "data": {
    "name": "Test Net",
    "hidden": 2,
    "neuronsPerHidden": 2
  },
  "respondId": 12 
}
```
respond:
```json
{
  "type": "respond",
  "what": "ok",
  "data": {
    "id": 2,
    "name": "Test Net"
  },
  "respondId": 12
}
```

#### Get one
```json
{
  "type": "request",
  "route": [
    {"datastructure": "2"},
    {"network": "4"}
  ],
  "what": "get",
  "respondId": 12 
}
```
respond:
```json
{
  "type": "respond",
  "what": "ok",
  "data": {
    "name": "Test Net",
    "learnRate": 0.001,
    "optimizer": "sgd"
  },
  "respondId": 12
}
```



## Do
#### Start train
```json
{
  "type": "request",
  "route": [
    {"datastructure": "2"},
    {"network":       "4"}
  ],
  "what": "do",
  "data": {
    "do": "startTrain|stopTrain",
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


## Chart
#### Update
```json
{
  "type": "request",
  "route": [
    {"datastructure": "2"},
    {"network":       "4"},
    {"chart":         "progress"}
  ],
  "what": "update",
  "data": {
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
  },
  "respondId": 12 
}
```
#### Register
Register chart updates for network (for unsubscribe:  ```"type": "unsubscribe"```)
```json
{
  "type": "request",
  "route": [
    {"datastructure": "2"},
    {"network":       "4"},
    {"chart":         ""}
  ],
  "what": "subscribe|unsubscribe",
  "respondId": 12 
}
```