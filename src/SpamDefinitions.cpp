#include <string>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <algorithm>
#include <fstream>

#include "SpamDefinitions.h"

const std::string SPAM_DEFINITIONS_FILENAME = "spam.txt";
const std::string HEURISTIC_SPAM_DEFINITIONS_FILENAME = "spam.heuristic.txt";

std::vector<boost::regex> LoadRegexPatternVectorFromFile(const std::string& filename)
{
  std::vector<boost::regex> patterns;

  std::ifstream patternsFile;
  patternsFile.open(filename.c_str());
  while(patternsFile.good())
  {
    std::string line;
    getline(patternsFile, line);
    if(line[0] == '#' || line == "")
    {
      continue;
    }
    patterns.push_back(boost::regex(line, boost::regex_constants::icase)); //Note: case insensitive
  }
  patternsFile.close();

  return patterns;
}

bool IsStringInRegexPatternVector(const std::vector<boost::regex>& patterns, const std::string& string)
{
  for(std::vector<boost::regex>::const_iterator patternItr = patterns.begin(); patternItr != patterns.end(); ++patternItr)
  {
    boost::smatch unused;
    if(boost::regex_search(string, unused, *patternItr))
    {
      return true;
    }
  }

  return false;
}

SpamDefinitions::SpamDefinitions()
{
  spamDefinitions = LoadRegexPatternVectorFromFile(SPAM_DEFINITIONS_FILENAME);
  heuristicSpamDefinitions = LoadRegexPatternVectorFromFile(HEURISTIC_SPAM_DEFINITIONS_FILENAME);
}

bool SpamDefinitions::CheckSpam(const std::string& message) const
{
  return IsStringInRegexPatternVector(spamDefinitions, message);
}

bool SpamDefinitions::CheckSpamHeuristic(const std::string& message) const
{
  return IsStringInRegexPatternVector(heuristicSpamDefinitions, message);
}
