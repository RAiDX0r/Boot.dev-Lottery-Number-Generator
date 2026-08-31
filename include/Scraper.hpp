#pragma once

#include <HtmlParser/Parser.hpp>
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
  std::vector<DrawResult> ParseHtml(const std::string& RawHtml) const;
};
