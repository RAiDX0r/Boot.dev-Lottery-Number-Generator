#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream> // 💡 Unlocks file stream structures

#include "DataStore.hpp"
#include "NetworkClient.hpp"
#include "Scraper.hpp"
#include "Types.hpp"

namespace fs = std::filesystem;

int main()
{
  std::cout << "=================================================" << std::endl;
  std::cout << "--- CANADIAN LOTTERY STREAM DIAGNOSTIC BENCH ---" << std::endl;
  std::cout << "=================================================" << std::endl;

  NetworkClient Client;
  Scraper WebScraper;
  DataStore Storage;

  std::string TargetUrl = "https://ca.lottonumbers.com/lotto-max/numbers/2026";
  LotteryGame SelectedGame = LotteryGame::LottoMax;

  std::cout << "\n[1/3] Downloading Live HTML Byte Stream..." << std::endl;
  std::string RawHtml = Client.DownloadPage(TargetUrl);

  if (RawHtml.empty() == true)
  {
    std::cerr << "-> [FAIL] Download returned an empty buffer." << std::endl;
    return 1;
  }
  std::cout << "-> [PASS] Received " << RawHtml.size() << " bytes of text data." << std::endl;

  // 🧪 💡 THE DUMP LOGIC: Save the raw data stream exactly as Lexbor receives it
  std::string DumpFilename = "debug_raw_stream.html";
  std::ofstream DumpFile(DumpFilename);
  
  if (DumpFile.is_open() == true)
  {
    DumpFile << RawHtml;
    DumpFile.close();
    std::cout << "\n-> [SUCCESS] Raw stream saved to disk!" << std::endl;
    std::cout << "   File Path: " << fs::absolute(DumpFilename) << std::endl;
  }
  else
  {
    std::cerr << "-> [ERROR] Failed to create the debug dump file." << std::endl;
  }

  std::cout << "\n[2/3] Executing Scraper Class Parser Pipeline..." << std::endl;
  std::vector<DrawResult> Results = WebScraper.ParseHtml(RawHtml, SelectedGame);
  std::cout << "-> Scraper count: Extracted " << Results.size() << " fully-formed records." << std::endl;

  std::cout << "\n[3/3] Synchronizing Storage Layer onto Hard Drive..." << std::endl;
  unsigned int SavedCount = 0;
  for (const auto& Draw : Results)
  {
    if (Storage.SaveDraw(Draw.GameType, Draw) == true)
    {
      SavedCount++;
    }
  }
  std::cout << "-> Sync Finished. Logged: " << SavedCount << " rows." << std::endl;

  std::cout << "\n=================================================" << std::endl;
  return 0;
}
