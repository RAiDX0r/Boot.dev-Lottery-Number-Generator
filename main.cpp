#include <iostream>
#include <fstream>
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
  
  // 1. Define all active games
  std::vector<LotteryGame> ActiveGames = {LotteryGame::LottoMax};
  // Later: { LotteryGame::LottoMax, LotteryGame::Lotto649 };

  NetworkClient Client;
  Scraper WebScraper;
  DataStore Storage;
  MetricsEngine Engine;
  ConsoleReporter Reporter;

  for (LotteryGame Game : ActiveGames)
  {
    Reporter.PrintSectionHeader("Processing: " + GameToString(Game));

    // 2. Download & Scrape
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

    // 3. Save to Disk
    for (const auto& Draw : Results)
    {
      Storage.SaveDraw(Game, Draw);
    }

    // 4. Load & Calculate Metrics
    std::vector<DrawResult> History = Storage.LoadAllDraws(Game);
    auto FreqMap = Engine.CalculateNumberFrequency(History, Game);
    auto SkipMap = Engine.CalculateBallSkipMetrics(History, Game);

    // 5. Report Dashboard
    Reporter.PrintFrequencyRanking(FreqMap, 5);
    Reporter.PrintSkipRanking(SkipMap, 5);
  }

  return 0;
}
