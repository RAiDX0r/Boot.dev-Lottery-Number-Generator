#include <iostream>
#include <string>
#include <vector>

#include "Types.hpp"
#include "DataStore.hpp"
#include "NetworkClient.hpp"
#include "Scraper.hpp"

int main()
{
  std::cout << "--- Canadian Lottery Strategy Tracker Engine ---" << std::endl;

  // 1. Initialize our stateless processing components
  NetworkClient Client;
  Scraper WebScraper;
  DataStore Storage;

  // 2. Configure targeted Canadian URLs (Recent page or Historical Year page)
  std::string TargetUrl = "https://ca.lottonumbers.com/lotto-max/numbers/2026";
  LotteryGame SelectedGame = LotteryGame::LottoMax;

  std::cout << "\n[Network] Downloading drawings from: " << TargetUrl << std::endl;
  std::string RawHtml = Client.DownloadPage(TargetUrl);

  // Explicit safety check
  if (RawHtml.empty() == true)
  {
    std::cerr << "[ERROR] Web connection failed or returned an empty page." << std::endl;
    return 1;
  }

  std::cout << "[Network] Success. Processing " << RawHtml.size() << " bytes of HTML text..." << std::endl;

  // 3. Execute extraction sequence - passing the correct game type context
  std::vector<DrawResult> ParsedDraws = WebScraper.ParseHtml(RawHtml, SelectedGame);
  std::cout << "[Scraper] Extracted " << ParsedDraws.size() << " raw drawings from webpage." << std::endl;

  // 4. Ingest and save unique records to their dedicated database tracks
  unsigned int SavedCount = 0;
  unsigned int SkippedCount = 0;

  std::cout << "\n[Storage] Synchronizing local database files..." << std::endl;
  for (const auto& Draw : ParsedDraws)
  {
    bool WasSaved = Storage.SaveDraw(Draw.GameType, Draw);

    if (WasSaved == true)
    {
      SavedCount++;
    }
    else
    {
      SkippedCount++;
    }
  }

  // 5. Finalize execution reporting metrics
  std::cout << "\n--- Sync Summary ---" << std::endl;
  std::cout << "Successfully logged: " << SavedCount << " new entries." << std::endl;
  std::cout << "Safely skipped:      " << SkippedCount << " duplicate records." << std::endl;
  std::cout << "--------------------" << std::endl;

  return 0;
}
