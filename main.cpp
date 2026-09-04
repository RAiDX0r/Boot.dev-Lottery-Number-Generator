#include <iostream>
#include <string>
#include <vector>

#include "DataStore.hpp"
#include "MetricsEngine.hpp"
#include "NetworkClient.hpp"
#include "Scraper.hpp"
#include "Types.hpp"

int main()
{
  std::cout << "=================================================" << std::endl;
  std::cout << "--- Full Pipeline & Metrics Verification ---" << std::endl;
  std::cout << "=================================================" << std::endl;

  NetworkClient Client;
  Scraper WebScraper;
  DataStore Storage;
  MetricsEngine Engine;

  LotteryGame SelectedGame = LotteryGame::LottoMax;
  std::string TargetUrl(LOTTO_MAX_TEST_URL);

  // -------------------------------------------------------
  // PHASE 1: Data Acquisition & Storage
  // -------------------------------------------------------
  std::cout << "\n[1/4] Downloading Live HTML Byte Stream..." << std::endl;
  std::string RawHtml = Client.DownloadPage(TargetUrl);

  if (RawHtml.empty() == true)
  {
    std::cerr << "-> [FAIL] Download returned an empty buffer." << std::endl;
    return 1;
  }
  std::cout << "-> [PASS] Received " << RawHtml.size() << " bytes." << std::endl;

  std::cout << "\n[2/4] Executing Scraper & Synchronizing Storage..." << std::endl;
  std::vector<DrawResult> ScrapedResults = WebScraper.ParseHtml(RawHtml, SelectedGame);

  unsigned int SavedCount = 0;
  for (const auto& Draw : ScrapedResults)
  {
    if (Storage.SaveDraw(Draw.GameType, Draw) == true)
    {
      SavedCount++;
    }
  }
  std::cout << "-> [PASS] Logged " << SavedCount << " records to CSV." << std::endl;

  // -------------------------------------------------------
  // PHASE 2: Metrics Engine Verification
  // -------------------------------------------------------
  std::cout << "\n[3/4] Loading Data & Running Frequency Analysis..." << std::endl;
  std::vector<DrawResult> HistoricalData = Storage.LoadAllDraws(SelectedGame);

  if (HistoricalData.empty() == true)
  {
    std::cerr << "-> [FAIL] No historical data found for metrics calculation." << std::endl;
    return 1;
  }

  auto FrequencyMap = Engine.CalculateNumberFrequency(HistoricalData, SelectedGame);

  std::cout << "-> Frequency Matrix Generated (" << FrequencyMap.size() << " unique balls tracked):" << std::endl;
  for (const auto& Pair : FrequencyMap)
  {
    std::cout << "   Ball [" << Pair.first << "]: " << Pair.second << " appearances" << std::endl;
  }

  std::cout << "\n[4/4] Running Skip Metrics Analysis..." << std::endl;
  auto SkipMap = Engine.CalculateBallSkipMetrics(HistoricalData, SelectedGame);

  std::cout << "-> Skip Metrics Generated:" << std::endl;
  for (const auto& Pair : SkipMap)
  {
    std::cout << "   Ball [" << Pair.first << "]: " << Pair.second << " games skipped" << std::endl;
  }

  std::cout << "\n=================================================" << std::endl;
  std::cout << "--- Verification Complete ---" << std::endl;
  std::cout << "=================================================" << std::endl;

  return 0;
}
