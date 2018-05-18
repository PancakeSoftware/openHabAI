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