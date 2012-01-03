#include <string>
#include <fstream>
#include <iostream>

#include "ChatLog.h"

ChatLog::ChatLog(const std::string& filename, const bool echoToConsole): echoToConsole(echoToConsole)
{
  logFile.open(filename.c_str());
}

ChatLog::~ChatLog()
{
  logFile.close();
}

void ChatLog::Log(const std::string& message)
{
  logFile << message << std::endl;

  if(echoToConsole)
  {
    std::cout << message << std::endl;
  }
}