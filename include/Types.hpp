#pragma once

#include <string>
#include <vector>

/// Used to help distinguish between game types
enum class LotteryGame
{
  LottoMax,
  Lotto649
};

// Centralised Trnaslation Helpers For LotteryGame
inline std::string GameToString(LotteryGame Game)
{
  switch (Game)
  {
    case LotteryGame::LottoMax:
      return "lotto-max";
    case LotteryGame::Lotto649:
      return "lotto-649";
    default:
      throw std::invalid_argument("Unknown game enum.");
  }
}

inline LotteryGame StringToGame(const std::string& GameString)
{
  if (GameString == "lotto-max")
  {
    return LotteryGame::LottoMax;
  }
  else if (GameString == "lotto-649")
  {
    return LotteryGame::Lotto649;
  }

  throw std::invalid_argument("Unknown game string token: " + GameString);
}

struct DrawResult
{
  /// The type of lottery game.
  LotteryGame GameType;

  /// The chronological date of the drawing in strict YYYY-MM-DD format.
  std::string Date;

  /// A collection holding the primary winning ball integers.
  std::vector<unsigned int> Numbers;

  /// The single, non-negative bonus ball number.
  unsigned int BonusNumber;
};
