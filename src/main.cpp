#include <csignal>
#include <iostream>

#include "MitmLogger.h"

MitmLogger mitmLogger;

bool forceAbort = false;
void HandleSigInt(int sig)
{
  if(forceAbort) //Appears all is not going smoothly with the graceful interrupt
  {
    abort();
  }
  else
  {
    mitmLogger.GracefulInterrupt();
    forceAbort = true; //For next time - there should not be a next time
  }
}

int main(int argc, char* argv[])
{
  std::signal(SIGINT, HandleSigInt);

  try
  {
    mitmLogger.Run();
  }
  catch(const std::exception& e)
  {
    std::cout << "Fatal: " << e.what() << std::endl;
  }
}