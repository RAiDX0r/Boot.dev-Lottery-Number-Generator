#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

inline constexpr std::string_view LOTTO_MAX_TEST_URL = "https://ca.lottonumbers.com/lotto-max/numbers/2026";

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

inline unsigned int GetBallCountForGame(LotteryGame Game)
{
  switch (Game)
  {
    case LotteryGame::LottoMax:
      return 8;
    case LotteryGame::Lotto649:
      return 7;
    default:
      throw std::invalid_argument("Unknown Game Type.");
  }
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

/**
 * @brief Safely checks if a string consists entirely of numeric characters.
 * @param Value The string variable to validate.
 * @return true if the string is non-empty and safe to convert using std::stoul.
 */
inline bool IsNumericString(const std::string& Value)
{
  using std::all_of;
  using std::isdigit;

  if (Value.empty() == true)
  {
    return false;
  }

  return all_of(Value.begin(), Value.end(), [](unsigned char Character)
  { 
    return isdigit(Character); 
  });
}
