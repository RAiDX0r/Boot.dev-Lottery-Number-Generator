#pragma once

#include <string>

/***
 * @brief Handles secure HTTP operations to fetch remote web contents.
 */
class NetworkClient
{
 public:
  /// @brief Downloads a webpage from the provided URL and returns a string representation of the page.
  /// @param TargetUrl The webpage to download.
  /// @return A string of the webpage that was downloaded.
  std::string DownloadPage(const std::string& TargetUrl);
};
