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

  for (unsigned int i = 1; i <= MAX_NUMBER; i++)
  {
    RC[i] = 0;
  }

  for (const DrawResult& Draw : AllDraws)
  {
    TempSet.clear();

    for (const unsigned int Number : Draw.Numbers)
    {
      TempSet.insert(Number);
    }

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
