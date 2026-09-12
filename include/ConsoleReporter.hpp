#pragma once

#include <map>
#include <string>

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
   * @brief Prints a formatted table of the top N most frequent ball numbers.
   *
   * Converts the input frequency map into a sorted vector (descending by
   * appearance count) and displays the highest-ranked entries in a
   * bordered terminal table.
   *
   * @param FrequencyMap A map where the Key is the ball number and the
   *                     Value is the total count of historical appearances.
   * @param TopN The number of top-ranked entries to display (e.g., 5).
   */
  void PrintFrequencyRanking(const std::map<unsigned int, unsigned int>& FrequencyMap, unsigned int TopN) const;

  /**
   * @brief Prints a formatted table of the top N coldest (highest skip) ball numbers.
   *
   * Converts the input skip map into a sorted vector (descending by skip
   * count) and displays the most "overdue" entries in a bordered terminal
   * table.
   *
   * @param SkipMap A map where the Key is the ball number and the Value
   *                is the number of games elapsed since its last appearance.
   * @param TopN The number of top-ranked entries to display (e.g., 5).
   */
  void PrintSkipRanking(const std::map<unsigned int, unsigned int>& SkipMap, unsigned int TopN) const;

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
  void PrintReportHeader(const std::string& FrequencyType, const std::string& ColumnHeader, const unsigned int TopN) const;

  /**
   * @brief Prints a bordered report data section.
   *
   * @param DataMap A map where the Key is the ball number and the Value
   *                is the metric appearance number.
   * @param TopN The number of top-ranked entries to display (e.g., 5).
   */
  void PrintReportData(const std::map<unsigned int, unsigned int>& DataMap, unsigned int TopN) const;
};
