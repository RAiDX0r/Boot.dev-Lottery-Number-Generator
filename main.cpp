#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

#include "ConsoleReporter.hpp"
#include "DataStore.hpp"
#include "MetricsEngine.hpp"
#include "NetworkClient.hpp"
#include "Scraper.hpp"
#include "Types.hpp"

// ============================================================
// Debug Flags: Adjust these to test different sources
// ============================================================

// Index into the games array in games.json
// 0 = lotto-max, 1 = lotto-649
constexpr int TEST_GAME_INDEX = 0;

// Index into the sources array for the selected game
// 0 = ca.lottonumbers.com (primary)
// 1 = lottodatabase.com (secondary)
constexpr int TEST_SOURCE_INDEX = 1;

// Year to download and test
constexpr unsigned int TEST_YEAR = 2026;

// Set to true to save the raw HTML to a debug file
constexpr bool SAVE_RAW_HTML = true;

// Set to true to print the first 5 extracted draws
constexpr bool PRINT_RESULTS = true;

// ============================================================

int main()
{
  std::cout << "=================================================" << std::endl;
  std::cout << "--- Source Verification Test Bench ---" << std::endl;
  std::cout << "=================================================" << std::endl;

  // 1. Load configuration
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

  // 2. Validate debug flag indices
  if (TEST_GAME_INDEX < 0 || TEST_GAME_INDEX >= static_cast<int>(Config.Games.size()))
  {
    std::cerr << "[FATAL] TEST_GAME_INDEX out of range." << std::endl;
    return 1;
  }

  const GameDefinition& GameDef = Config.Games[TEST_GAME_INDEX];

  if (TEST_SOURCE_INDEX < 0 || TEST_SOURCE_INDEX >= static_cast<int>(GameDef.Sources.size()))
  {
    std::cerr << "[FATAL] TEST_SOURCE_INDEX out of range." << std::endl;
    return 1;
  }

  const GameSource& Source = GameDef.Sources[TEST_SOURCE_INDEX];

  std::cout << "\n-> Game:   " << GameDef.DisplayName << " (" << GameDef.Id << ")" << std::endl;
  std::cout << "-> Source:  " << Source.BaseUrl << std::endl;
  std::cout << "-> Anchor:  " << Source.DateSelector << std::endl;
  std::cout << "-> Balls:   " << Source.BallSelector << std::endl;
  std::cout << "-> Date:    " << Source.DateStrategy << std::endl;
  std::cout << "-> Year:    " << TEST_YEAR << std::endl;
  std::cout << "-> BallCount: " << GameDef.BallCount << std::endl;

  // 3. Build URL and download
  std::string TargetUrl = Source.BaseUrl + std::to_string(TEST_YEAR);
  std::cout << "\n[1/3] Downloading: " << TargetUrl << std::endl;

  NetworkClient Client;
  std::string RawHtml = Client.DownloadPage(TargetUrl);

  if (RawHtml.empty() == true)
  {
    std::cerr << "-> [FAIL] Download returned empty response." << std::endl;
    return 1;
  }

  std::cout << "-> [PASS] Received " << RawHtml.size() << " bytes." << std::endl;

  // 4. Save raw HTML for browser comparison
  if (SAVE_RAW_HTML == true)
  {
    std::string DebugPath = "debug_" + GameDef.Id + "_" + Source.DateStrategy + "_" + std::to_string(TEST_YEAR) + ".html";
    std::ofstream DebugFile(DebugPath);

    if (DebugFile.is_open() == true)
    {
      DebugFile << RawHtml;
      DebugFile.close();
      std::cout << "-> [DEBUG] Raw HTML saved to: " << DebugPath << std::endl;
    }
    else
    {
      std::cerr << "-> [WARN] Could not open debug file for writing." << std::endl;
    }
  }

  // 5. Parse with Scraper
  std::cout << "\n[2/3] Parsing HTML with Lexbor..." << std::endl;

  Scraper WebScraper;
  std::vector<DrawResult> Results = WebScraper.ParseHtml(RawHtml, GameDef, Source);

  std::cout << "-> Extracted " << Results.size() << " draw records." << std::endl;

  if (Results.empty() == true)
  {
    std::cerr << "-> [WARN] No results extracted. Check selectors and date strategy." << std::endl;
    std::cerr << "-> [HINT] Open the debug HTML file in a browser and verify the selectors." << std::endl;
    return 1;
  }

  // 6. Print first N results for verification
  if (PRINT_RESULTS == true)
  {
    std::cout << "\n[3/3] First " << (Results.size() < 5 ? Results.size() : 5) << " extracted draws:" << std::endl;
    std::cout << "+------------+------------------------------------------+----------+" << std::endl;
    std::cout << "|    Date    |              Numbers                     |  Bonus   |" << std::endl;
    std::cout << "+------------+------------------------------------------+----------+" << std::endl;

    unsigned int PrintCount = (Results.size() < 5) ? Results.size() : 5;

    for (unsigned int i = 0; i < PrintCount; ++i)
    {
      const DrawResult& Draw = Results[i];
      std::string NumberString;

      for (const unsigned int Num : Draw.Numbers)
      {
        if (!NumberString.empty())
        {
          NumberString += " ";
        }
        NumberString += std::to_string(Num);
      }

      std::cout << "| " << std::left << std::setw(10) << Draw.Date
                << "| " << std::left << std::setw(36) << NumberString
                << "| " << std::right << std::setw(8) << Draw.BonusNumber << " |" << std::endl;
    }

    std::cout << "+------------+------------------------------------------+----------+" << std::endl;
  }

  std::cout << "\n=================================================" << std::endl;
  std::cout << "--- Test Complete ---" << std::endl;
  std::cout << "=================================================" << std::endl;

  return 0;
}
