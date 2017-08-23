/* 
 * File:   Filesystem.h
 * Author: Joshua Johannson
 *
  */
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <iostream>
#include "util/Filesystem.h"
using namespace Filesystem;

bool Filesystem::lsFor(string path, function<void(string name, bool isDir)> onEntry)
{
  if (onEntry == NULL)
    return false;

  Filesystem::createDirIfNotExists(path);

  DIR *dir = opendir(path.c_str());
  if (dir == NULL)
    return false;

  struct dirent *entry = readdir(dir);

  for (; entry != NULL; entry = readdir(dir))
  {

    if (strcmp(entry->d_name, ".") == 0 or strcmp(entry->d_name, "..") == 0)
    {
      continue;
    }

    onEntry(entry->d_name, entry->d_type == DT_DIR);
  }

  closedir(dir);
  return true;
}

bool ::Filesystem::createDirIfNotExists(string path)
{
  // check if folder exists
  struct stat statbuf;
  int isDir = 0;
  if (stat(path.c_str(), &statbuf) == -1) {
    if (!S_ISDIR(statbuf.st_mode)) {
      cout << "create dir " << path << endl;
      if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0)
        cout << "cant create dir " << path << endl;
    }
  }
}
