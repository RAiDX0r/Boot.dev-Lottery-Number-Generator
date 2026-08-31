#pragma once

#include <string>
#include <vector>

/// Used to help distinguish between game types
enum class LotteryGame
{
  LottoMax,
  Lotto649
};

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



