/* 
 * File:   JsonObject.h
 * Author: Joshua Johannson
 *
  */
#include "util/JsonObject.h"


// -- TO FROM JSON --------------------------------------------------------
Json JsonObject::toJson()
{
  Json j{};
  for (pair<string, boost::any> el : paramPointers) {

    if (el.second.type() == typeid(int*))
      j[el.first] = *boost::any_cast<int *>(el.second);
    else if (el.second.type() == typeid(string*))
      j[el.first] = *boost::any_cast<string *>(el.second);
    else if (el.second.type() == typeid(float*))
      j[el.first] = *boost::any_cast<float *>(el.second);
    else if (el.second.type() == typeid(double *))
      j[el.first] =  *boost::any_cast<double *>(el.second);
    else if (el.second.type() == typeid(bool *))
      j[el.first] =  *boost::any_cast<bool *>(el.second);

  }

  return j;
}

void JsonObject::fromJson(Json params)
{
  for (Json::iterator it = params.begin(); it != params.end(); ++it) {
    auto i = paramPointers.find(it.key());
    if (i == paramPointers.end())
      continue;

    pair<string, boost::any> el = *i;

    if (el.second.type() == typeid(int*))
      *boost::any_cast<int *>(el.second) = it.value();
    else if (el.second.type() == typeid(string*))
      *boost::any_cast<string *>(el.second) = it.value();
    else if (el.second.type() == typeid(float*))
      *boost::any_cast<float *>(el.second) = it.value();
    else if (el.second.type() == typeid(double *))
      *boost::any_cast<double *>(el.second) = it.value();
    else if (el.second.type() == typeid(bool *))
      *boost::any_cast<bool *>(el.second) = it.value();
  }
}


// -- SAVE LOAD --------------------------------------------------------
bool JsonObject::save(string path, string fileName)
{
  return false;
}

bool JsonObject::load(string path, string fileName)
{
  return false;
}

void JsonObject::loadChilds()
{

}



void JsonObject::setJsonParams(map<string, boost::any>  params)
{
  this->paramPointers = params;
}
