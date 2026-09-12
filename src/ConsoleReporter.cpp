#include "ConsoleReporter.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

void ConsoleReporter::PrintFrequencyRanking(const std::map<unsigned int, unsigned int>& FrequencyMap, unsigned int TopN) const
{
}

void ConsoleReporter::PrintSkipRanking(const std::map<unsigned int, unsigned int>& SkipMap, unsigned int TopN) const
{
}

void ConsoleReporter::PrintSectionHeader(const std::string& Title) const
{
  std::cout << "=================================================" << std::endl;
  std::cout << "--- " << Title << " ---" << std::endl;
  std::cout << "=================================================" << std::endl;
}

void ConsoleReporter::PrintReportHeader(const std::string& FrequencyType, const std::string& ColumnHeader, const unsigned int TopN) const
{
  std::cout << "\n[ Top " << TopN << " " << FrequencyType << " Numbers ]" << std::endl;
  std::cout << "+------+-----------------+" << std::endl;
  std::cout << "| " << std::left << std::setw(4) << "Ball"
            << " | " << std::right << std::setw(15) << ColumnHeader << " |" << std::endl;
  std::cout << "+------+-----------------+" << std::endl;
}

void ConsoleReporter::PrintReportData(const std::map<unsigned int, unsigned int>& DataMap, unsigned int TopN) const
{
  std::vector<std::pair<const unsigned int, unsigned int>> SortedBalls(DataMap.begin(), DataMap.end());

  for (unsigned int i = 0; i < TopN && i < SortedBalls.size(); ++i)
  {
    std::cout << "| " << std::left << std::setw(4) << SortedBalls[i].first
              << " | " << std::right << std::setw(15) << SortedBalls[i].second << " |" << std::endl;
  }
  std::cout << "+------+-----------------+" << std::endl;
}
