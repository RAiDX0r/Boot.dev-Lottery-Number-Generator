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

int main()
{
  constexpr bool DEBUG_SAVE_HTML = true;

  std::vector<LotteryGame> ActiveGames = {LotteryGame::LottoMax};

  NetworkClient Client;
  Scraper WebScraper;
  DataStore Storage;
  MetricsEngine Engine;
  ConsoleReporter Reporter;

  for (LotteryGame Game : ActiveGames)
  {
    Reporter.PrintSectionHeader("Processing: " + GameToString(Game));

    std::string RawHtml = Client.DownloadPage(GetGameUrl(Game));

    if (DEBUG_SAVE_HTML == true)
    {
      std::ofstream DebugFile("debug_raw_stream.html");
      if (DebugFile.is_open() == true)
      {
        DebugFile << RawHtml;
        DebugFile.close();
        std::cout << "-> [DEBUG] Raw HTML saved to debug_raw_stream.html" << std::endl;
      }
    }

    std::vector<DrawResult> Results = WebScraper.ParseHtml(RawHtml, Game);

    unsigned int SavedCount = 0;
    for (const auto& Draw : Results)
    {
      if (Storage.SaveDraw(Game, Draw) == true)
      {
        SavedCount++;
      }
    }
    std::cout << "-> Synced " << SavedCount << " new records to disk." << std::endl;

    std::vector<DrawResult> History = Storage.LoadAllDraws(Game);
    if (History.empty() == true)
    {
      std::cerr << "-> [SKIP] No historical data available for metrics." << std::endl;
      continue;
    }

    auto FreqMap = Engine.CalculateNumberFrequency(History, Game);
    auto SkipMap = Engine.CalculateBallSkipMetrics(History, Game);
    auto CalendarStats = Engine.CalculateCalendarFrequency(History, Game);

    std::cout << "\n-> Calendar Split: " << CalendarStats.first << " (1-31) / "
              << CalendarStats.second << " (32-Max)" << std::endl;

    Reporter.PrintRanking(FreqMap, 5, ReportVerbiage::Frequent);
    Reporter.PrintRanking(SkipMap, 5, ReportVerbiage::Infrequent);
  }

  return 0;
}
