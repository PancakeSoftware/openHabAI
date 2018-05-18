/* 
 * File:   Filesystem.h
 * Author: Joshua Johannson
 *
  */
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <functional>
using namespace std;


/*
 * Filesystem
 */
namespace Filesystem
{
  bool lsFor(string path, function<void (string, bool)> onEntry); // list contend of folder, call onEntry function

  bool createDirIfNotExists(string path);
};


#endif //FILESYSTEM_H
