#include <iostream>
#include <string>

#include "NetworkClient.hpp"

int main()
{
  std::cout << "--- NetworkClient Test Bench ---" << std::endl;

  // 1. Initialize your new web download component
  NetworkClient Client;

  // 2. Point it to a real web target link
  std::string TargetUrl = "https://ca.lottonumbers.com/lotto-max/numbers/2026";
  std::cout << "Connecting to: " << TargetUrl << std::endl;

  // 3. Fire the downloader engine pipeline
  std::string RawHtml = Client.DownloadPage(TargetUrl);

  // 4. Verify we successfully pulled down text data
  if (!RawHtml.empty())
  {
    std::cout << "\nSUCCESS: Web page data pulled into memory RAM!" << std::endl;
    std::cout << "Total characters downloaded: " << RawHtml.size() << " bytes." << std::endl;

    // 💡 THE TEST: Print out the final 100 characters of the downloaded text
    std::cout << "\n--- Web Document Ending Preview ---" << std::endl;
    size_t StartPosition = RawHtml.size() > 100 ? RawHtml.size() - 100 : 0;
    std::cout << RawHtml.substr(StartPosition) << std::endl;
    std::cout << "-----------------------------------" << std::endl;
  }
  else
  {
    std::cout << "\nFAILURE: The download buffer returned empty." << std::endl;
  }

  return 0;
}
