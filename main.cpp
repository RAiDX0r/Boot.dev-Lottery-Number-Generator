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

#include <iostream>
#include "DataStore.hpp"
#include "Types.hpp"

int main()
{
  std::cout << "--- DataStore Test Bench ---" << std::endl;

  // 1. Create our database tool pointing to a local file string
  DataStore store("lotto_history.csv");

  // 2. Create a test drawing bundle matching our Types.hpp layout
  DrawResult testDraw;
  testDraw.Date = "2026-08-25";
  testDraw.Numbers = {7, 14, 21, 28, 35, 42, 49};
  testDraw.BonusNumber = 13;

  // 3. Trigger our save command (which will run our duplicate check!)
  std::cout << "Attempting to save draw..." << std::endl;
  bool success = store.SaveDraw(testDraw);

  if (success)
  {
    std::cout << "SUCCESS: New numbers saved!" << std::endl;
  }
  else
  {
    std::cout << "SKIPPED: That draw already exists." << std::endl;
  }

  return 0;
}
