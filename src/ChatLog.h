#pragma once
#include <string>
#include <fstream>

class ChatLog
{
  private:
  std::ofstream logFile;

  bool echoToConsole;

  public:
  ChatLog(const std::string& filename, const bool echoToConsole = false);
  ~ChatLog();

  void Close();

  void Log(const std::string& message);
};