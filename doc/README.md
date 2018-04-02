# Api between backend and frontend
## Basic packet structure
There are two main types of packages:

1.  ##### Request
    ```json
    {
      "type": "request",
      "route": "Component-A/entityID/Sub-Component/entityID",
  
      "what": "add|update|remove|get|getAll|do|...",
      "data": {
  
      },
      "respondId": 12 
    }
    ```
    *   **route** defines something similar to rest url, in this example you want to access:<br>
        ```entityID-A of Component-A / entityID-B of Sub-Component```
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
  "route": "dataStructures/",
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
  "route": "dataStructures/",
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
  "route": "dataStructures/<id>/networks",
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
  "route": "dataStructures/<id>/networks",
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
  "route": "dataStructures/<id>/networks/<id>",
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
  "route": "dataStructures/<id>/networks/<id>",
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
send to server
```json
{
  "type": "request",
  "route": "dataStructures/<id>/networks/<id>/charts/progress",
  "what": "update",
  "data": {
      "fixedInputs": [
          {"id": 0, "value": 10},
          {"id": 1, "value": 5}
      ],
      "rangeInputs": [
          {"id": 2, "from": 10, "to": 30,   "steps": 2},
          {"id": 3, "from": 110, "to": 130, "steps": 2}
      ],
      "outputs": [1, 2]
    },
  "respondId": 12 
}
```
#### Register
Register chart updates for network (for unsubscribe:  ```"type": "unsubscribe"```)
```json
{
  "type": "request",
  "route": "dataStructures/<id>/networks/<id>/charts/progress",
  "what": "subscribe|unsubscribe",
  "respondId": 12 
}
```