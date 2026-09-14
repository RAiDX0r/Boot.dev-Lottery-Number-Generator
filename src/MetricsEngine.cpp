// Calculates metrics based on the extracted data.
#include "MetricsEngine.hpp"

#include <map>
#include <set>
#include <vector>

#include "Types.hpp"

std::map<unsigned int, unsigned int> MetricsEngine::CalculateNumberFrequency(const std::vector<DrawResult>& AllDraws, [[maybe_unused]] LotteryGame Game) const
{
  std::map<unsigned int, unsigned int> RC;

  for (const DrawResult& Draw : AllDraws)
  {
    for (const unsigned int Number : Draw.Numbers)
    {
      RC[Number]++;
    }
  }

  return RC;
}

std::map<unsigned int, unsigned int> MetricsEngine::CalculateBallSkipMetrics(const std::vector<DrawResult>& AllDraws, LotteryGame Game) const
{
  const unsigned int MAX_NUMBER = GetMaxNumberForGame(Game);
  std::map<unsigned int, unsigned int> RC;
  std::set<unsigned int> TempSet;
  std::set<unsigned int> RecordedBalls;
  unsigned int GameIndex = 0;

  // Set RC to a safe-empty state
  for (unsigned int i = 1; i <= MAX_NUMBER; i++)
  {
    RC[i] = 0;
  }

  /** Iterate through each ball in the current draw
   *  Using a set is a lot more performant as the are set up a binary tree
   *  and the find() method is a binary search.
   *  Without a Set (Linear Search): If you checked against Draw.Numbers (a std::vector),
   *  the computer would have to scan through all 7 numbers one by one to find a match.
   *  This is O(N) complexity. Cost per draw: 52 balls × 7 checks = 364 operations.
   *  With a Set (Tree Lookup): A std::set is a balanced binary search tree.
   *  It can find a number in half the remaining space with each step.
   *  This is O(log N) complexity. Cost per draw: 52 balls × ~3 checks = ~156 operations.
   */
  for (const DrawResult& Draw : AllDraws)
  {
    TempSet.clear();

    for (const unsigned int Number : Draw.Numbers)
    {
      TempSet.insert(Number);
    }

    // For each possible number find how long ago it last occured
    for (unsigned int i = 1; i <= MAX_NUMBER; i++)
    {
      if (TempSet.find(i) != TempSet.end() && RecordedBalls.find(i) == RecordedBalls.end())
      {
        RC[i] = GameIndex;
        RecordedBalls.insert(i);
      }
    }

    GameIndex++;
  }

  return RC;
}

std::pair<unsigned int, unsigned int> MetricsEngine::CalculateCalendarFrequency(const std::vector<DrawResult>& AllDraws, LotteryGame Game) const
{
  const unsigned int MAX_NUMBER = GetMaxNumberForGame(Game);
  std::pair<unsigned int, unsigned int> RC;

  RC.first = 0;
  RC.second = 0;

  for (const DrawResult& Draw : AllDraws)
  {
    for (const unsigned int Number : Draw.Numbers)
    {
      if (Number <= 31)
      {
        RC.first++;
      }
      else
      {
        RC.second++;
      }
    }
  }

  return RC;
}
