#pragma once

#include <map>
#include <vector>

#include "Types.hpp"

/**
 * @brief This is a stateless utility class. It holds no internal data members. It acts purely as a mathematical transformation layer: it accepts raw historical data and returns computed statistical maps.
 */
class MetricsEngine
{
 public:
  /**
   * @brief Determine how many times each specific ball number has appeared across all historical draws
   *
   * @param AllDraws The complete history of lottery results.
   * @param Game Lottery game type to determine the valid range of numbers.
   * @return std::map<unsigned int, unsigned int> A map where the Key is the ball number and the Value is the total count of appearances.
   */
  std::map<unsigned int, unsigned int> CalculateNumberFrequency(const std::vector<DrawResult>& AllDraws, LotteryGame Game) const;

  /**
   * @brief Determine how many games have passed since each ball number last appeared. This identifies "overdue" numbers.
   *
   * @param AllDraws The complete history of lottery results.
   * @param Game Lottery game type to determine the total pool size.
   * @return std::map<unsigned int, unsigned int> A map where the Key is the ball number and the Value is the number of draws skipped since its last appearance.
   */
  std::map<unsigned int, unsigned int> CalculateBallSkipMetrics(const std::vector<DrawResult>& AllDraws, LotteryGame Game) const;

  /**
   * @brief Returns the total hits for Calendar (1-31) vs Non-Calendar (32-Max) balls.
   * @return std::pair<unsigned int, unsigned int> {CalendarHits, NonCalendarHits}
   */
  std::pair<unsigned int, unsigned int> CalculateCalendarFrequency(const std::vector<DrawResult>& AllDraws, LotteryGame Game) const;
};
