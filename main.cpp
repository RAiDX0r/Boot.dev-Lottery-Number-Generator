#include <iostream>
#include <string>
#include <vector>

#include "DataStore.hpp"
#include "NetworkClient.hpp"
#include "Scraper.hpp"
#include "Types.hpp"

int main()
{
  std::cout << "--- Canadian Lottery Strategy Tracker Engine ---" << std::endl;

  // 1. Initialize our stateless processing components
  NetworkClient Client;
  Scraper WebScraper;
  DataStore Storage;

  // 2. Fetch live data from the web target
  std::string TargetUrl = "https://ca.lottonumbers.com/lotto-max/numbers/2026";
  std::cout << "\n[Network] Downloading drawings from: " << TargetUrl << std::endl;
  std::string RawHtml = Client.DownloadPage(TargetUrl);

  // Explicit safety check matching your preferred style
  if (RawHtml.empty() == true)
  {
    std::cerr << "[ERROR] Web connection failed or returned an empty page." << std::endl;
    return 1;
  }

  std::cout << "[Network] Success. Processing " << RawHtml.size() << " bytes of HTML text..." << std::endl;

  // 3. Execute the extraction algorithm sequence
  std::vector<DrawResult> ParsedDraws = WebScraper.ParseHtml(RawHtml);
  std::cout << "[Scraper] Extracted " << ParsedDraws.size() << " raw drawings from webpage." << std::endl;

  // 4. Ingest and save unique records to their dedicated database tracks
  unsigned int SavedCount = 0;
  unsigned int SkippedCount = 0;

  std::cout << "\n[Storage] Synchronizing local database files..." << std::endl;
  for (const auto& Draw : ParsedDraws)
  {
    // Pass the extracted Draw's GameType enum straight to the storage engine
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
