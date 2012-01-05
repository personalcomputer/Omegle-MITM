#include <string>
#include <set>
#include <boost/lexical_cast.hpp>
#include <ctime>
#include <functional>
#include <algorithm>
#include <vector>
#include <iostream>
#include "Omegle.h"

#include "Error.h"
#include "SpamDefinitions.h"
#include "ChatLog.h"

#include "ChatRoom.h"

const time_t TIMEOUT_BEFORE_RETRY_TIME = 60*6; //in seconds
const time_t ADDITIONAL_TIMEOUT_BEFORE_RETRY_TIME_PER_MESSAGE = 20; //in seconds //Basically, if you have a long conversation, the participants can sit idle for a longer period of time without triggering the idle timeout and thus ending the conversation.

/*void FriendlyDisconnect(Omegle::Connection* stranger)
{
  // To attempt to avoid spam detection.

  stranger->SendTyping();
  usleep(500000);
  stranger->SendMessage("ok");
  usleep(500000);
  stranger->SendTyping();
  usleep(500000);
  stranger->SendMessage("bye");

  stranger->Disconnect();
}*/

ChatRoom::ChatRoom(const SpamDefinitions& spamDefinitions, std::set<Omegle::ServerId_t>& failedServers): messageCount(0), lastActivity(time(NULL)), spamDefinitions(spamDefinitions), chatLog(NULL)
{
  std::set<Omegle::ServerId_t> additionalAvoidServers;

  for(int strangerNumber = 1; strangerNumber <= 2; ++strangerNumber) //2 = 2 participants. The groupchat version should not block while connecting participants like it does here
  {
    Omegle::ServerId_t connectedServer;
    strangers.push_back(Stranger(strangerNumber, false, NewConnection(additionalAvoidServers, failedServers, connectedServer)));
    additionalAvoidServers.insert(connectedServer); //Avoid making multiple connections to the same server. Omegle seems to detect that and more readily hand out CAPTCHAs.
  }
}

ChatRoom::~ChatRoom()
{
  Disconnect();
}

Omegle::Connection* ChatRoom::NewConnection(const std::set<Omegle::ServerId_t>& additionalAvoidServers, std::set<Omegle::ServerId_t>& failedServers, Omegle::ServerId_t& connectedServer)
{
  // Avoid problematic or less-desirable servers
  for(Omegle::ServerId_t serverIterator = 0; serverIterator < Omegle::SERVER_COUNT; ++serverIterator)
  {
    if(failedServers.count(serverIterator) || additionalAvoidServers.count(serverIterator))
    {
      continue;
    }

    try
    {
      connectedServer = serverIterator;
      return new Omegle::Connection(serverIterator);
    }
    catch(Omegle::CaptchaError){}
    catch(Omegle::NetworkError){}

    //There was an error
    failedServers.insert(serverIterator);
  }

  std::set<Omegle::ServerId_t> untestedAvoidServers;
  std::set_difference(additionalAvoidServers.begin(), additionalAvoidServers.end(), failedServers.begin(), failedServers.end(), std::inserter(untestedAvoidServers, untestedAvoidServers.end()));

  for(std::set<Omegle::ServerId_t>::iterator serverIterator = untestedAvoidServers.begin(); serverIterator != untestedAvoidServers.end(); ++serverIterator)
  {
    try
    {
      connectedServer = *serverIterator;
      return new Omegle::Connection(*serverIterator);
    }
    catch(Omegle::CaptchaError){}
    catch(Omegle::NetworkError){}

    //There was an error
    failedServers.insert(*serverIterator);
  }

  throw Error("Omegle servers exhausted");
}

void ChatRoom::AddChatLog(ChatLog* const chatLog)
{
  this->chatLog = chatLog;
}

void ChatRoom::ForEachStrangerButOne(const std::vector<Stranger>::iterator exclude, std::function<void(Stranger&)> f)
{
  for(std::vector<Stranger>::iterator itr = strangers.begin(); itr != strangers.end(); ++itr)
  {
    if(itr != exclude)
    {
      f(*itr);
    }
  }
}

void ChatRoom::HandleEvents()
{
  for(std::vector<Stranger>::iterator strangerItr = strangers.begin(); strangerItr != strangers.end(); ++strangerItr)
  {
    Omegle::PacketId packetId;
    Omegle::Packet* packet;

    while(strangerItr->connection->PollEvent(&packetId, &packet, Omegle::NONBLOCKING))
    {
      std::string strangerName = "Stranger "+boost::lexical_cast<std::string>(strangerItr->number);
      
      if(packetId == Omegle::PID_STRANGERMESSAGE)
      {
        std::string message = static_cast<Omegle::StrangerMessagePacket*>(packet)->message;

        chatLog->Log(strangerName+": "+message);

        // Stop from repeating spam
        if(spamDefinitions.CheckSpam(message))
        {
          Disconnect();
          chatLog->Log("(disconnected - spam filtering");
          return;
        }

        if(!strangerItr->hasSpoken)
        {
          // Use aggressive spam filtering for the first message

          if(spamDefinitions.CheckSpamHeuristic(message))
          {
            Disconnect();
            chatLog->Log("(disconnected - spam filtering (heuristic))");
            return;
          }
          strangerItr->hasSpoken = true;
        }

        messageCount++;

        ForEachStrangerButOne(strangerItr, [&](Stranger& partner)
        {
          partner.connection->SendMessage(message);
        });
      }
      else if(packetId == Omegle::PID_STARTTYPING)
      {
        ForEachStrangerButOne(strangerItr, [&](Stranger& partner)
        {
          partner.connection->SendTyping();
        });
      }
      else if(packetId == Omegle::PID_STOPTYPING)
      {
        ForEachStrangerButOne(strangerItr, [&](Stranger& partner)
        {
          partner.connection->SendStopTyping();
        });
      }
      else if(packetId == Omegle::PID_DISCONNECT)
      {
        delete strangerItr->connection;
        strangers.erase(strangerItr);
        Disconnect(); 
        chatLog->Log("("+strangerName+" disconnected)");
        return;
      }

      lastActivity = time(NULL);
    }
  }

  if((time(NULL) - lastActivity) >= TIMEOUT_BEFORE_RETRY_TIME + ADDITIONAL_TIMEOUT_BEFORE_RETRY_TIME_PER_MESSAGE*messageCount)
  {
    Disconnect();
    chatLog->Log("(disconnected - idle timeout)");
    return;
  }
}

void ChatRoom::Disconnect()
{
  while(!strangers.empty())
  {
    strangers.back().connection->Disconnect();
    delete strangers.back().connection;
    strangers.pop_back();
  }
}

bool ChatRoom::IsConnected() const
{
  return !strangers.empty();
}
