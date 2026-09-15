#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "ConsoleReporter.hpp"
#include "DataStore.hpp"
#include "MetricsEngine.hpp"
#include "NetworkClient.hpp"
#include "Scraper.hpp"
#include "Types.hpp"


/*

Secondary Website
https://www.lottodatabase.com/lotto-database/canadian-lotteries/lotto-max/draw-history/2009

*/





int main()
{
  constexpr bool DEBUG_SAVE_HTML = false;
  constexpr unsigned int END_YEAR = 2026;

  std::cout << "=================================================" << std::endl;
  std::cout << "--- Canadian Lottery Strategy Tracker ---" << std::endl;
  std::cout << "=================================================" << std::endl;

  // 1. Load Data-Driven Configuration
  LotteryConfig Config;
  try
  {
    Config = LoadGameConfig("games.json");
  }
  catch (const std::exception& Ex)
  {
    std::cerr << "[FATAL] " << Ex.what() << std::endl;
    return 1;
  }

  if (Config.Games.empty() == true)
  {
    std::cerr << "[FATAL] No games defined in games.json." << std::endl;
    return 1;
  }

  std::cout << "-> Loaded " << Config.Games.size() << " game(s) from configuration." << std::endl;

  NetworkClient Client;
  Scraper WebScraper;
  DataStore Storage;
  MetricsEngine Engine;
  ConsoleReporter Reporter;

  // 2. Iterate through each configured game
  for (const auto& GameDef : Config.Games)
  {
    Reporter.PrintSectionHeader("Processing: " + GameDef.DisplayName);

    // 3. Download & Scrape all available years
    unsigned int TotalSaved = 0;

    for (unsigned int Year = GameDef.StartYear; Year <= END_YEAR; ++Year)
    {
      std::string TargetUrl = GameDef.BaseUrl + std::to_string(Year);
      std::cout << "-> Fetching: " << TargetUrl << std::endl;

      std::string RawHtml = Client.DownloadPage(TargetUrl);

      if (RawHtml.empty() == true)
      {
        std::cerr << "   -> [SKIP] Empty response for year " << Year << std::endl;
        continue;
      }

      if (DEBUG_SAVE_HTML == true)
      {
        std::string DebugPath = "debug_" + GameDef.Id + "_" + std::to_string(Year) + ".html";
        std::ofstream DebugFile(DebugPath);

        if (DebugFile.is_open() == true)
        {
          DebugFile << RawHtml;
          DebugFile.close();
        }
      }

      std::vector<DrawResult> Results = WebScraper.ParseHtml(RawHtml, GameDef);

      for (const auto& Draw : Results)
      {
        if (Storage.SaveDraw(GameDef, Draw) == true)
        {
          TotalSaved++;
        }
      }

      std::cout << "   -> Extracted " << Results.size() << " records." << std::endl;
    }

    std::cout << "-> Total new records synced: " << TotalSaved << std::endl;

    // 4. Load Full History & Calculate Metrics
    std::vector<DrawResult> History = Storage.LoadAllDraws(GameDef);

    if (History.empty() == true)
    {
      std::cerr << "-> [SKIP] No historical data available for " << GameDef.DisplayName << std::endl;
      continue;
    }

    std::cout << "-> Total historical records in memory: " << History.size() << std::endl;

    auto FreqMap = Engine.CalculateNumberFrequency(History, GameDef);
    auto SkipMap = Engine.CalculateBallSkipMetrics(History, GameDef);
    auto CalendarStats = Engine.CalculateCalendarFrequency(History, GameDef);

    std::cout << "\n-> Calendar Split: " << CalendarStats.first << " (1-31) / "
              << CalendarStats.second << " (32-Max)" << std::endl;

    // 5. Report Dashboard
    Reporter.PrintRanking(FreqMap, 5, ReportVerbiage::Frequent);
    Reporter.PrintRanking(SkipMap, 5, ReportVerbiage::Infrequent);
  }

  std::cout << "\n=================================================" << std::endl;
  std::cout << "--- All Games Processed Successfully ---" << std::endl;
  std::cout << "=================================================" << std::endl;

  return 0;
}
