#pragma once

#include "Types.hpp"

#include <string>
#include <vector>

/**
 * @brief Takes a raw HTML string from the NetworkClient class and extracts data using HTMLParser.
 * 
 */
class Scrapper
{
public:
 std::vector<DrawResult> ParseHtml() const;
};
