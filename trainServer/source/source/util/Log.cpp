/* 
 * File:   Log.h
 * Author: Joshua Johannson
 *
  */
#include <Frontend.h>
#include "util/Log.h"


// static
char Log::logLevelMask    = LOG_LEVEL_ALL;
bool Log::useLongLogNames = false;
bool Log::useColors       = false;
int  Log::maxNameLongLen  = 0;


// -- SET LOG NAME ----------------------
void Log::setLogName(string nameShort)
{
  this->logNameShort = nameShort;
  this->logNameLong  = "";
}
void Log::setLogName(string nameShort, string nameLong)
{
  setLogName(nameShort);
  this->logNameLong = nameLong;

  if (logNameLong.length() > maxNameLongLen)
    maxNameLongLen = logNameLong.length();
}

void Log::setLogLevel(char log_levelMask)
{
  logLevelMask = log_levelMask;
}


void Log::setUseLongLogNames(bool use)
{
  useLongLogNames = use;
}

void Log::setUseColor(bool use)
{
  useColors = use;
}


// -- OUT -------------------------------
void Log::fatal(string text)
{
  print("[fatal ] ", text, LOG_LEVEL_ERROR);
  Frontend::send("fatal", {{"message",  text + "''"}});
}

void Log::fatal(string text, string error)
{
  print("[fatal ] ", text + ": '" + error + "'", LOG_LEVEL_ERROR);
  Frontend::send("fatal", {{"message",  text + "'" + error + "'"}});
}

void Log::err(string text)
{
  if ((logLevelMask & LOG_LEVEL_ERROR) > 0)
    print("[ err  ] ", text, LOG_LEVEL_ERROR);
}

void Log::err(string text, string error)
{
  if ((logLevelMask & LOG_LEVEL_ERROR) > 0)
    print("[ err  ] ", text + ": '" + error + "'", LOG_LEVEL_ERROR);
}

void Log::warn(string text)
{
  if ((logLevelMask & LOG_LEVEL_WARNING) > 0)
    print("[ warn ] ", text, LOG_LEVEL_WARNING);
}

void Log::ok(string text)
{
  if ((logLevelMask & LOG_LEVEL_OK) > 0)
    print("[  ok  ] ", text, LOG_LEVEL_OK);
}

void Log::debug(string text)
{
  if ((logLevelMask & LOG_LEVEL_DEBUG) > 0)
    print("[debug ] ", text, LOG_LEVEL_DEBUG);
}

void Log::info(string text)
{
  if ((logLevelMask & LOG_LEVEL_INFORMATION) > 0)
    print("[ info ] ", text, LOG_LEVEL_INFORMATION);
}

void Log::trace(string text)
{
  if ((logLevelMask & LOG_LEVEL_TRACE) > 0)
    print("[ trac ] ", text, LOG_LEVEL_TRACE);
}

void Log::outErr(string text, bool error, string errorText)
{
  if (error && ((logLevelMask & LOG_LEVEL_ERROR) > 0))
    print("[ err  ] ", text + ": '" + errorText + "'", LOG_LEVEL_ERROR);
  else
    this->ok(text);
}

void Log::outErr(string text, bool error)
{
  outErr(text, error, "");
}

void Log::outFat(string text, bool error, string errorText)
{
  if (error && ((logLevelMask & LOG_LEVEL_ERROR) > 0))
    print("[fatal ] ", text + ": '" + errorText + "'", LOG_LEVEL_ERROR);
  else
    this->ok(text);
}

void Log::outFat(string text, bool error)
{
  outErr(text, error, "");
}



void Log::print(string tag, string text, char tagMask)
{
  // output
  if (!useLongLogNames)
    cout << "[ " << logNameShort.substr(0, 4) << " ] " << tag << "" << text << endl;
  else
  {
    if (logNameLong != "")
      cout << "[ " << logNameLong.substr(0, maxNameLongLen) << " ] " << tag << text  << endl;
    else
      cout << "[ " << logNameShort.substr(0, maxNameLongLen) << " ] " << tag << text << endl;

  }
}
