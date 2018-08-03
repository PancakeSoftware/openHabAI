/* 
 * File:   Util
 * Author: Joshua Johannson
 *
  */
#ifndef OPENHABAI_CATFLOW_UTIL_H
#define OPENHABAI_CATFLOW_UTIL_H
#include <exception>
#include <string>

class JsonObjectException: public std::exception {
  private:
    std::string message;
  public:
    explicit JsonObjectException(const std::string& message) : message(message) {}

    virtual const char* what() const throw() {
      return message.c_str();
    }
};

#endif //OPENHABAI_CATFLOW_UTIL_H
