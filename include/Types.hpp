#pragma once

#include <algorithm>
#include <cctype>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct DrawResult
{
  /// The chronological date of the drawing in strict YYYY-MM-DD format.
  std::string Date;

  /// A collection holding the primary winning ball integers.
  std::vector<unsigned int> Numbers;

  /// The single, non-negative bonus ball number.
  unsigned int BonusNumber;
};

struct GameDefinition
{
  std::string Id;
  std::string DisplayName;
  std::string BaseUrl;
  unsigned int BallCount;
  unsigned int MaxNumber;
  unsigned int StartYear;
};

struct LotteryConfig
{
  std::vector<GameDefinition> Games;
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
                { return isdigit(Character); });
}

inline LotteryConfig LoadGameConfig(const std::string& FilePath)
{
  LotteryConfig Config;
  std::ifstream File(FilePath);

  if (File.is_open() == false)
  {
    throw std::runtime_error("Failed to open config file: " + FilePath);
  }

  nlohmann::json JsonData = nlohmann::json::parse(File);

  for (const auto& GameJson : JsonData["games"])
  {
    GameDefinition GameDef;
    GameDef.Id = GameJson["id"];
    GameDef.DisplayName = GameJson["display_name"];
    GameDef.BaseUrl = GameJson["base_url"];
    GameDef.BallCount = GameJson["ball_count"];
    GameDef.MaxNumber = GameJson["max_number"];
    GameDef.StartYear = GameJson["start_year"];

    Config.Games.push_back(GameDef);
  }

  return Config;
}
