#pragma once
#include <string>
#include <set>
#include <vector>
#include "Omegle.h"

class MitmLogger
{
  private:
  bool operatorInterruption;

  bool isLogging; //In the middle of logging a conversation? This is used for thread/signal synchronization.

  std::set<Omegle::ServerId_t> failedServers;

  unsigned int GetHighestPreexistingLogNumber();

  public:
  MitmLogger();

  void Run();

  void GracefulInterrupt();
};