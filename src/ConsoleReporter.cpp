#include "ConsoleReporter.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

void ConsoleReporter::PrintRanking(const std::map<unsigned int, unsigned int>& Map, unsigned int TopN, ReportVerbiage ReportType) const
{
  ReportVerbiage Title;
  ReportVerbiage ColumnHeader;

  switch (ReportType)
  {
    case ReportVerbiage::Frequent:
      Title = ReportVerbiage::FrequentTitle;
      ColumnHeader = ReportVerbiage::FrequentColumnHeader;
      break;

    case ReportVerbiage::Infrequent:
      Title = ReportVerbiage::InfrequentTitle;
      ColumnHeader = ReportVerbiage::InfrequentColumnHeader;
      break;

    default:
      break;
  }

  this->PrintReportHeader(TopN, Title, ColumnHeader);
  this->PrintReportData(Map, TopN);
}

void ConsoleReporter::PrintSectionHeader(const std::string& Title) const
{
  std::cout << "=================================================" << std::endl;
  std::cout << "--- " << Title << " ---" << std::endl;
  std::cout << "=================================================" << std::endl;
}

void ConsoleReporter::PrintReportHeader(const unsigned int TopN, const ReportVerbiage Title, const ReportVerbiage ColumnHeader) const
{
  std::cout << "\n[ Top " << TopN << " " << GetReportString(Title) << " Numbers ]" << std::endl;
  std::cout << "+------+-----------------+" << std::endl;
  std::cout << "| " << std::left << std::setw(4) << "Ball"
            << " | " << std::right << std::setw(15) << GetReportString(ColumnHeader) << " |" << std::endl;
  std::cout << "+------+-----------------+" << std::endl;
}

void ConsoleReporter::PrintReportData(const std::map<unsigned int, unsigned int>& DataMap, unsigned int TopN) const
{
  // Pair .first connot be const as it will cause compiler errors
  std::vector<std::pair<unsigned int, unsigned int>> SortedBalls(DataMap.begin(), DataMap.end());
  std::sort(SortedBalls.begin(), SortedBalls.end(), [](const auto& First, const auto& Second)
            { return First.second > Second.second; });

  for (unsigned int i = 0; i < TopN && i < SortedBalls.size(); ++i)
  {
    std::cout << "| " << std::left << std::setw(4) << SortedBalls[i].first
              << " | " << std::right << std::setw(15) << SortedBalls[i].second << " |" << std::endl;
  }
  std::cout << "+------+-----------------+" << std::endl;
}
