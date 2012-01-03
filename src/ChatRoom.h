#pragma once
#include <string>
#include <set>
#include <ctime>
#include <functional>
#include <vector>
#include "Omegle.h"

#include "ChatLog.h"

struct Stranger
{
  unsigned int number;
  bool hasSpoken;

  Omegle::Connection* connection;

  inline Stranger(const unsigned int number, const bool hasSpoken, Omegle::Connection* connection): number(number), hasSpoken(hasSpoken), connection(connection) {}
};

class ChatRoom
{
  private:
  std::vector<Stranger> strangers;

  unsigned int messageCount;

  time_t lastActivity;

  const std::vector<std::string> spamDictionary;

  ChatLog* chatLog;

  Omegle::Connection* NewConnection(const std::set<Omegle::ServerId_t>& additionalAvoidServers, std::set<Omegle::ServerId_t>& failedServers, Omegle::ServerId_t& connectedServer); //Add a new chat participant. //failedServers is input and output. connectedServer is output.

  void ForEachStrangerButOne(const std::vector<Stranger>::iterator exclude, std::function<void(Stranger&)> f);

  public:
  ChatRoom(const std::vector<std::string>& spamDictionary,
           std::set<Omegle::ServerId_t>& failedServers); /*, //failedServers is both input and output.
           const bool persistent = false, //Does the chatroom persist when someone disconnects (and attempt to re-fill that position), or does it restart and acquire totally new participants.
           const bool groupchatMode = false, //Alert all participants to what is going on and prepends messages with the real sender's id.
           const unsigned int participantsCount = 2); //I do not recommend going above Omegle::SERVER_COUNT */

  ~ChatRoom();

  void AddChatLog(ChatLog* const chatLog);

  void HandleEvents();

  void Disconnect(); //Disconnects chat participants.

  bool IsConnected() const; //If one or both strangers are disconnected (in non-persistent mode), the chat is considered 'disconnected' and cannot be resumed. In persistent mode, it will only be unconnected if all strangers leave and all Omegle servers have us blocked (CAPTCHAs).
};