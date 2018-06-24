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

template<class T>
bool internal::JsonParam<T>::isPrimitive() {
  if constexpr (std::is_base_of<JsonObject, T>::value) {
    return false;
  }
  else
    return true;
}


template<class T>
bool internal::JsonParamReadOnly<T>::isPrimitive() {
  if constexpr (std::is_base_of<JsonObject, T>::value) {
    return false;
  }
  else
    return true;
}
