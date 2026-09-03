#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "DataStore.hpp"
#include "NetworkClient.hpp"
#include "Scraper.hpp"
#include "Types.hpp"

namespace fs = std::filesystem;

int main()
{
  std::cout << "=================================================" << std::endl;
  std::cout << "--- Canadian Lottery Strategy Tracker Engine ---" << std::endl;
  std::cout << "=================================================" << std::endl;

  NetworkClient Client;
  Scraper WebScraper;
  DataStore Storage;

  // 🎯 REFERENCE CENTRAL TRUTH: Converts compile-time view to local string natively
  std::string TargetUrl(LOTTO_MAX_TEST_URL);
  LotteryGame SelectedGame = LotteryGame::LottoMax;

  std::cout << "\n[1/3] Downloading Live HTML Byte Stream..." << std::endl;
  std::cout << "-> Target URL Target: " << TargetUrl << std::endl;

  std::string RawHtml = Client.DownloadPage(TargetUrl);

  if (RawHtml.empty() == true)
  {
    std::cerr << "-> [FAIL] Download returned an empty buffer." << std::endl;
    return 1;
  }
  std::cout << "-> [PASS] Received " << RawHtml.size() << " bytes of text data." << std::endl;

  std::cout << "\n[2/3] Executing Scraper Class Parser Pipeline..." << std::endl;
  std::vector<DrawResult> Results = WebScraper.ParseHtml(RawHtml, SelectedGame);
  std::cout << "-> Scraper count: Extracted " << Results.size() << " fully-formed records." << std::endl;

  std::cout << "\n[3/3] Synchronizing Storage Layer onto Hard Drive..." << std::endl;
  unsigned int SavedCount = 0;
  unsigned int SkippedCount = 0;

  for (const auto& Draw : Results)
  {
    if (Storage.SaveDraw(Draw.GameType, Draw) == true)
    {
      SavedCount++;
    }
    else
    {
      SkippedCount++;
    }
  }

  std::cout << "-> Sync Finished. Logged: " << SavedCount << " | Skipped: " << SkippedCount << std::endl;

  return 0;
}
