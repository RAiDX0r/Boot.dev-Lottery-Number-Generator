#pragma once

#include <string>
#include <vector>

#include "Types.hpp"

/**
 * @brief Takes a raw HTML string from the NetworkClient class and extracts data using HTMLParser.
 *
 */
class Scraper
{
 public:
  /**
   * @brief Parses lottery drawing pages (both recent results and archives) into structured DrawResult records.
   *
   * @param RawHtml The raw web markup text string data downloaded by the network client.
   * @param Game The explicit lottery game context parameter currently being targeted.
   * @return A standard vector collection holding every unique, fully-populated DrawResult struct.
   */
  std::vector<DrawResult> ParseHtml(const std::string& RawHtml, LotteryGame Game) const;

 private:
  std::string GetElementAttribute(lxb_dom_element_t* Element, const std::string& AttrName) const;
};
