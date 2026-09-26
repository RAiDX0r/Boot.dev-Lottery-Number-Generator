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
  std::optional<std::string> ParseDate(lxb_dom_element_t* DateElement, const std::string& Strategy, std::string_view DateParseString) const;

  /**
   * @brief Tokenises a date template string into its constituent literal and identifier segments.
   *
   * Given a template like "{DotW}, {Month} {Day}, {Year}", this method locates
   * each identifier, then slices the string into the alternating segments that
   * make it up. The returned vector preserves left-to-right order:
   *
   *   ["{DotW}", ", ", "{Month}", " ", "{Day}", ", ", "{Year}"]
   *
   * Odd-indexed elements are identifiers; even-indexed elements are the literal
   * text between them. (This holds when the template begins with an identifier.)
   *
   * Any identifier not found in the template is skipped.
   *
   * @param DateParseString The template pattern (from games.json "date_parse_string").
   * @param Identifiers     The four placeholder tokens to search for.
   * @return Ordered vector of segments comprising the template.
   */
  std::vector<std::string_view> TokenisePattern(std::string_view DateParseString, const std::array<std::string_view, 4> Identifiers) const;

  /**
   * @brief Takes the month part and returns the two digit month as a string.
   *
   * @param Month The string view to process representing the extracted month.
   * @return std::string Two digit month.
   */
  std::string ProcessMonthToMM(const std::string_view Month) const;

  /**
   * @brief Safe, zero-allocation case-insensitive equality check.
   *
   * @param a First character.
   * @param b Second character.
   * @return true a is the same case-insensitive letter as b.
   * @return false a is not the same case-insensitive letter as b.
   */
  bool Equals(std::string_view a, std::string_view b) const;
};
