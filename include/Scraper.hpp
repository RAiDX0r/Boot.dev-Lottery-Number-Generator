#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Types.hpp"

struct lxb_dom_element;
typedef struct lxb_dom_element lxb_dom_element_t;

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
   * @param GameDef The explicit lottery game context parameter currently being targeted.
   * @param GameSource Contains information on how to parse a particular game.
   * @return A standard vector collection holding every unique, fully-populated DrawResult struct.
   */
  std::vector<DrawResult> ParseHtml(const std::string& RawHtml, const GameDefinition& GameDef, const GameSource& Source) const;

 private:
  /**
   * @brief Get the HTML Element Attribute value.
   *
   * @param Element The HTML Element to parse.  (Lexbor object)
   * @param AttrName The HTML Tag's attribute name to look under.
   * @return std::string The value of the Attribute for the Element.
   */
  std::string GetElementAttribute(lxb_dom_element_t* Element, const std::string& AttrName) const;

  /**
   * @brief Extracts and normalizes a draw date from a DOM element into YYYY-MM-DD format.
   *
   * The extraction method is determined by the Strategy parameter:
   * - "url_segment": reads the element's href attribute, splits on '/',
   *   locates the segment following "numbers", and takes the first 10 characters.
   * - "text_parse": extracts the element's inner text, then slices it using
   *   the placeholder tokens anchored by the DateParseString template string.
   *
   * @param DateElement The Lexbor DOM element that carries the date information.
   * @param Strategy The date extraction method ("url_segment" or "text_parse").
   * @param DateParseString The date_parse_string template from games.json (e.g. "{DotW}, {Month} {Day}, {Year}").
   *               Ignored when Strategy is "url_segment".
   * @return std::optional<std::string> The normalized date in YYYY-MM-DD format,
   *         or std::nullopt if the date could not be extracted or is malformed.
   */
  std::optional<std::string> ParseDate(lxb_dom_element_t* DateElement, const std::string& Strategy, const std::string& DateParseString) const;
};
