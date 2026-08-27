#include <filesystem>
#include <iostream>

#include "DataStore.hpp"
#include "Types.hpp"

int main()
{
  std::cout << "--- DataStore Test Bench ---" << std::endl;
  std::filesystem::path RootPath = std::filesystem::current_path();
  std::filesystem::path CsvPath = RootPath / "lottery_history.csv";

  std::cout << "[DEBUG] Target CSV Path: " << CsvPath.string() << std::endl;

  // 1. Create our database tool pointing to a local file string
  DataStore Store(CsvPath.string());

  // 2. Create a test drawing bundle matching our Types.hpp layout
  DrawResult TestDraw;
  TestDraw.Date = "2026-08-25";
  TestDraw.Numbers = {7, 14, 21, 28, 35, 42, 49};
  TestDraw.BonusNumber = 13;

  // 3. Trigger our save command (which will run our duplicate check!)
  std::cout << "Attempting to save draw..." << std::endl;
  bool Success = Store.SaveDraw(TestDraw);

  if (Success)
  {
    std::cout << "SUCCESS: New numbers saved!" << std::endl;
  }
  else
  {
    std::cout << "SKIPPED: That draw already exists." << std::endl;
  }

  return 0;
}


// #include <iostream>
// #include <string>
// #include <vector>
// #include "HtmlParser/Parser.hpp"

// int main()
// {
//   std::cout << "Boot.dev Lottery Number Generator Initialized!" << std::endl;

//   // Test code simulating our future target lottery layout
//   std::string SampleHtml = "<ul class='balls'><li>5</li><li>22</li></ul>";

//   HtmlParser::Parser Parser;

//   // The library uses a custom HtmlParser::DOM type container
//   HtmlParser::DOM Dom = Parser.Parse(SampleHtml);

//   // Grab elements using the built-in tag name finder
//   auto ballLists = Dom.GetElementsByTagName("ul");

//   std::cout << "Verified parser function. Lists found: " << ballLists.size() << std::endl;
//   return 0;
// }
