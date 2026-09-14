#pragma once

#include <map>
#include <string>

#include "Types.hpp"

/**
 * @brief A stateless utility class responsible for formatting and printing
 *        lottery metrics to the command-line terminal.
 *
 * This class handles all presentation logic, including sorting statistical
 * maps into ranked vectors and applying fixed-width formatting for clean
 * dashboard alignment.
 */
class ConsoleReporter
{
 public:
  /**
   * @brief Prints a formatted table of the top N ball numbers based on the report type.
   *
   * @param Map A map where the Key is the ball number and the Value is the
   *            occurances based on the ReportType.
   * @param TopN The number of top-ranked entries to display (e.g. 5).
   * @param ReportType The type of report to print.
   */
  void PrintRanking(const std::map<unsigned int, unsigned int>& Map, unsigned int TopN, ReportVerbiage ReportType) const;

  /**
   * @brief Prints a centered, bordered section header to separate dashboard
   *        segments.
   *
   * @param Title The descriptive text to display within the header borders.
   */
  void PrintSectionHeader(const std::string& Title) const;

 private:
  /**
   * @brief Prints a bordered report header to signify the start of a report.
   *
   * @param FrequencyType The type of frequency to show in the report title.
   * @param ColumnHeader The title for the second column in the report.
   * @param TopN The number of top-ranked entries to display (e.g., 5).
   */
  void PrintReportHeader(const unsigned int TopN, const ReportVerbiage Title, const ReportVerbiage ColumnHeader) const;

  /**
   * @brief Prints a bordered report data section.
   *
   * @param DataMap A map where the Key is the ball number and the Value
   *                is the metric appearance number.
   * @param TopN The number of top-ranked entries to display (e.g., 5).
   */
  void PrintReportData(const std::map<unsigned int, unsigned int>& DataMap, unsigned int TopN) const;
};
