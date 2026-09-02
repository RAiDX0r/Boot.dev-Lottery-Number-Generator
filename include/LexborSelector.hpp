#pragma once

#include <lexbor/css/css.h>
#include <lexbor/selectors/selectors.h>

#include <string>

#include "LexborCollection.hpp"
#include "LexborDocument.hpp"



/**
 * Query Translation Machine
 * Lexbor requires you to explicitly spin up multiple separate parser engines 
 * and memory pools just to process a string like "tr". This class wraps that 
 * heavy machinery behind a single clean C++ method: QuerySelect. It takes a 
 * string, compiles the code rules, executes the search, and automatically 
 * scrubs the temporary rule tracks to prevent data leaks.
 */

/**
 * @brief RAII safe memory management wrapper around Lexbor's CSS compilation and selection engines.
 */
class LexborSelector
{
 public:
  /**
   * @brief Constructs a new selector machine and initializes the underlying C memory contexts.
   */
  LexborSelector();

  /**
   * @brief Destructor. Destroys the compiled CSS context pools and selector machines cleanly.
   */
  ~LexborSelector();

  /**
   * @brief Executes a CSS selector query against a document context tree and populates a collection.
   * @param Document The active managed HTML document to search inside.
   * @param CssQuery The CSS selector text string (e.g. "table.past-results tr").
   * @param OutputCollection The managed collection container object to load with matching node results.
   * @return true if the query successfully compiled and ran, false if an optimization or parsing error occurred.
   */
  bool QuerySelect(const LexborDocument& Document, const std::string& CssQuery, LexborCollection& OutputCollection);

 private:
  // Low-level C structures required to run Lexbor's styling match routines
  lxb_css_parser_t* m_CssParserContext;
  lxb_selectors_t* m_SelectorEngineContext;
};
