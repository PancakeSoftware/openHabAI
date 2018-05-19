## Catflow example
Here are some example packages you can send via webSocket to the example-api:

##### Adding a course
```json
>> Send:
{
  "route": "/courses/",
  "what": "add",
  "data": {
    "courseName": "Math"
  },
  "respondId": 10
}

>> Receive:
{
  "data": {
    "courseName": "Math",
    "id": 2
  },
  "respondId": 10,
  "type": "respond",
  "what": "ok"
}
```

##### Adding a student for math course
```json
>> Send:
{
  "route": "/courses/2/students",
  "what": "add",
  "data": {
    "name": "lena",
    "age": 25
  },
  "respondId": 10
}

>> Receive:
{
  "data": {
    "age": 25,
    "id": 0,
    "name": "lena"
  },
  "respondId": 10,
  "type": "respond",
  "what": "ok"
}
```

##### Get list of all courses
```json
>> Send:
{
  "route": "/courses/",
  "what": "getAll",
  "respondId": 10
}

>> Receive:
{
  "data": [
      {
        "courseName": "softwareEngineering",
        "id": 1
      }, {
        "courseName": "math",
        "id": 2
      }
  ],
  "respondId": 10,
  "type": "respond",
  "what": "ok"
}
```

##### Get one student
```json
>> Send:
{
  "route": "/courses/2/students/0",
  "what": "get",
  "respondId": 10
}

>> Receive:
{
  "data": {
    "age": 25,
    "id": 0,
    "name": "lena"
  },
  "respondId": 10,
  "type": "respond",
  "what": "ok"
}
```

##### Listen for changes of student
```json
>> Send:
{
  "route": "/courses/2/students/0",
  "what": "subscribe"
}

>> Receive after '/courses/2/students/0' age changed:
{
  "data": {
    "age": 26
  },
  "route": "/courses/2/students/0/",
  "type": "request",
  "what": "update"
}
```

##### Update students age
```json
>> Send:
{
  "route": "/courses/2/students/0",
  "what": "update",
  "data": {
    "age": 26
  }
}
```