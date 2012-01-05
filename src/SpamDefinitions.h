#pragma once
#include <string>
#include <boost/regex.hpp>
#include <vector>

class SpamDefinitions
{
  private:
  std::vector<boost::regex> spamDefinitions;
  std::vector<boost::regex> heuristicSpamDefinitions;

  public:
  SpamDefinitions();

  bool CheckSpam(const std::string& message) const;
  bool CheckSpamHeuristic(const std::string& message) const;
};