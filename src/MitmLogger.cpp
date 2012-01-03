#include <string>
#include <set>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <iostream>
#include "Omegle.h"

#include "ChatRoom.h"
#include "ChatLog.h"

#include "Error.h"

#include "MitmLogger.h"

const std::string SPAM_FILENAME = "spam.txt";

const unsigned int BUSYWAIT_SLEEP_TIME = 60; //in miliseconds
const unsigned int INTERCONVO_SLEEP_TIME = 1000*3; //in miliseconds

MitmLogger::MitmLogger(): operatorInterruption(false), isLogging(false)
{
  
}

std::vector<std::string> MitmLogger::LoadSpam()
{
  std::vector<std::string> spam;

  std::ifstream spamFile;
  spamFile.open(SPAM_FILENAME.c_str());
  while(spamFile.good())
  {
    std::string line;
    getline(spamFile, line);
    if(line[0] == '#' || line == "")
    {
      continue;
    }
    spam.push_back(line);
  }
  spamFile.close();

  return spam;
}

unsigned int MitmLogger::GetHighestPreexistingLogNumber()
{
  unsigned int logNum = 0;

  for(boost::filesystem::directory_iterator itr("."); itr != boost::filesystem::directory_iterator(); ++itr)
  {
    boost::smatch regexFilenameMatch;

    if(boost::regex_match((*itr).path().filename().string(), regexFilenameMatch, boost::regex("log\\.(\\d+)\\.txt")))
    {
      unsigned int fileLogNum = boost::lexical_cast<unsigned int>(regexFilenameMatch[1]);

      if(fileLogNum > logNum)
      {
        logNum = fileLogNum;
      }
    }
  }

  return logNum;
}

void MitmLogger::Run()
{
  while(true)
  {
    unsigned int logNum = GetHighestPreexistingLogNumber() + 1;

    std::string logName = boost::lexical_cast<std::string>(logNum);
    logName.insert(logName.begin(), 5-logName.size(), '0'); //pad with 0s
    logName = "log."+logName+".txt";

    std::cout << "(connecting..)" << std::endl;
    ChatRoom chat(LoadSpam(), failedServers);
    std::cout << "(connected. logging into " << logName << ")" << std::endl;
    
    ChatLog chatLog(logName, true);

    chat.AddChatLog(&chatLog);

    isLogging = true;
    
    while(chat.IsConnected())
    {
      usleep(BUSYWAIT_SLEEP_TIME*1000);

      if(operatorInterruption)
      {
        chat.Disconnect();
        chatLog.Log("(disconnected - interrupted by operator)");
        exit(0);
      }

      try
      {
        chat.HandleEvents();
      }
      catch(Omegle::NetworkError)
      {
        chat.Disconnect();
        chatLog.Log("(disconnected - network failure)");
        exit(0);
      }
    }
  
    isLogging = false;

    std::cout << "(sleeping to avoid bot detection)" << std::endl;
    usleep(INTERCONVO_SLEEP_TIME*1000);
  }
}

void MitmLogger::GracefulInterrupt()
{
  if(isLogging)
  {
    operatorInterruption = true;
  }
  else
  {
    exit(0);
  }
}
