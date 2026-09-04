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
    std::string Html = Client.DownloadPage(GetGameUrl(Game));
    std::vector<DrawResult> Results = WebScraper.ParseHtml(Html, Game);

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
