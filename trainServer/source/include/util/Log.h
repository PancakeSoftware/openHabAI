/* 
 * File:   Log.h
 * Author: Joshua Johannson
 *
  */
#ifndef OPENHABAI_LOG_H
#define OPENHABAI_LOG_H


// include
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
using namespace std;


/*
 * Log class
 */
class Log
{
  public:
    Log();
    Log(string nameShort);

    static void setLogLevel(char log_level_mask);
    static void setUseLongLogNames(bool use);
    static void setUseColor(bool use);

    void setLogName(string nameShort); // set log name example: class name
    void setLogName(string nameShort, string nameLong); // set log name example: class name

    void fatal(string text);  // error message
    void fatal(string text, string error);  // error message
    void err(string text);  // error message
    void err(string text, string error);  // error message
    void warn(string text); // warning message
    void ok(string text);   // ok message
    void debug(string text);// debug message
    void info(string text) const; // information message
    void trace(string text);// trace info message

    void outErr(string text, bool error, string errorText); // ok/err - print error if error=true
    void outErr(string text, bool error);                   // ok/err - print error if error=true
    void outFat(string text, bool error, string errorText); // ok/fatal - print error if error=true
    void outFat(string text, bool error);                   // ok/fatal - print error if error=true

    static const char
        LOG_LEVEL_FATAL         = 0b00000000,
        LOG_LEVEL_ERROR         = 0b00000001,
        LOG_LEVEL_WARNING       = 0b00000010,
        LOG_LEVEL_OK            = 0b00000100,
        LOG_LEVEL_DEBUG         = 0b00001000,
        LOG_LEVEL_INFORMATION   = 0b00010000,
        LOG_LEVEL_TRACE         = 0b00100000,
        LOG_LEVEL_ALL           = 0b11111111;

  private:
    string logNameShort, logNameLong;
    static int logNameShortLongest;
    static char logLevelMask;
    static bool useLongLogNames, useColors;
    static int  maxNameLongLen;

    void print(string tag, string text, char tagMask) const;
};


#endif //OPENHABAI_LOG_H
