/*
 * File:   JsonObject.cpp
 * Author: Pancake Software
 */
#include "JsonObject.h"

template<class T>
void internal::JsonParam<T>::fromJson(Json j)
{
  if constexpr (std::is_base_of<JsonObject, T>::value) {
    (dynamic_cast<JsonObject*>(valuePtr))->fromJson(j);
  }
  else
    *valuePtr = j.get<T>();
}