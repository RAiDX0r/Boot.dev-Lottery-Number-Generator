#pragma once

#include <map>
#include <string>

/**
 * @brief A stateless ConsoleReporter utility class to handle all presentation logic.
 * 
 */
class ConsoleReporter
{
 public:
  // Prints a formatted table of the top N most frequent numbers
  void PrintFrequencyRanking(const std::map<unsigned int, unsigned int>& FrequencyMap, unsigned int TopN) const;

  // Prints a formatted table of the top N coldest (highest skip) numbers
  void PrintSkipRanking(const std::map<unsigned int, unsigned int>& SkipMap, unsigned int TopN) const;

  // Utility for clean console separation
  void PrintSectionHeader(const std::string& Title) const;
};
