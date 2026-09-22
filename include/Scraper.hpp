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
   * @param Game The explicit lottery game context parameter currently being targeted.
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

  std::optional<std::string> ParseDate(lxb_dom_element_t* DateElement, const std::string& Strategy, const std::string& Format) const;
};
