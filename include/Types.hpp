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

  // String that represents the type of lottery game.
  std::string GameType;
};

struct GameSource
{
  std::string BaseUrl;
  std::string AnchorSelector;
  std::string BallSelector;
  std::string DateStrategy;
};

struct GameDefinition
{
  std::string Id;
  std::string DisplayName;
  std::vector<GameSource> Sources;
  unsigned int BallCount;
  unsigned int MaxNumber;
  unsigned int StartYear;
};

struct LotteryConfig
{
  std::vector<GameDefinition> Games;
};

/**
 * @brief Used to limit
 *
 */
enum class CalendarPolicy
{
  Exclude,      // Strictly 32-Max
  Allow,        // Full 1-Max pool
  RestrictHalf  // Max 3 numbers from 1-31
};

enum class ReportVerbiage
{
  Frequent,
  Infrequent,
  FrequentTitle,
  InfrequentTitle,
  FrequentColumnHeader,
  InfrequentColumnHeader
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
    GameDef.BallCount = GameJson["ball_count"];
    GameDef.MaxNumber = GameJson["max_number"];
    GameDef.StartYear = GameJson["start_year"];

    for (const auto& SourceData : GameJson["sources"])
    {
      GameSource NewGameSource;
      NewGameSource.BaseUrl = SourceData["base_url"];
      NewGameSource.AnchorSelector = SourceData["anchor_selector"];
      NewGameSource.BallSelector = SourceData["ball_selector"];
      NewGameSource.DateStrategy = SourceData["date_strategy"];
      GameDef.Sources.push_back(NewGameSource);
    }

    Config.Games.push_back(GameDef);
  }

  return Config;
}

inline constexpr std::string_view GetReportString(ReportVerbiage Type)
{
  switch (Type)
  {
    case ReportVerbiage::FrequentTitle:
      return "Hottest";
    case ReportVerbiage::InfrequentTitle:
      return "Coldest";
    case ReportVerbiage::FrequentColumnHeader:
      return "Appearances";
    case ReportVerbiage::InfrequentColumnHeader:
      return "Games Skipped";
    default:
      return "";
  }
}
