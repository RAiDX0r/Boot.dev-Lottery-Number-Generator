#include "LexborSelector.hpp"

#include <iostream>

/**
 * Bridge Function
 * Pure C libraries cannot talk directly to advanced C++ classes. This static helper function acts
 * as a translator bridge. When the selector engine finds a matching HTML row on the webpage, it
 * throws it to this callback. The callback catches the raw node, re-interprets its memory shape,
 * and neatly appends it straight into your storage tray (LexborCollection).
 */

/**
 * @brief Static callback bridge that intercepts matched DOM nodes and appends them to a collection context.
 *
 * This function satisfies Lexbor's low-level functional callback layout. It acts as an
 * intermediary, extracting the raw node pointer, casting it to a standard element format,
 * and pushing it into our modern managed collection tray.
 *
 * @param Node The raw underlying DOM node discovered by the CSS query matching loop engine.
 * @param Spec The calculation ranking metric tracking the exact matching rules of the CSS selector string.
 * @param Ctx A raw untyped context handle smuggling the pointer location of our LexborCollection target.
 * @return LXB_STATUS_OK if the item was successfully appended, or an explicit Lexbor failure code.
 */
static lxb_status_t LexborCollectionAppendCallback(lxb_dom_node_t* Node, lxb_css_selector_specificity_t Spec, void* Ctx)
{
  // The 'Ctx' argument passes our raw lxb_dom_collection_t handle through the API
  lxb_dom_collection_t* Collection = reinterpret_cast<lxb_dom_collection_t*>(Ctx);

  // Safely cast the raw node base to a formal element layout and append it
  return lxb_dom_collection_append(Collection, lxb_dom_interface_element(Node));
}
LexborSelector::LexborSelector()
{
  // 1. Spin up a new low-level CSS rule compiler context instance
  this->m_CssParserContext = lxb_css_parser_create();

  // 2. Spin up the primary query match selector execution machine
  this->m_SelectorEngineContext = lxb_selectors_create();

  if (this->m_CssParserContext != nullptr)
  {
    // Initialize the parser's internal memory pools for compiling strings
    lxb_css_parser_init(this->m_CssParserContext, nullptr);
  }
}

LexborSelector::~LexborSelector()
{
  if (this->m_CssParserContext != nullptr)
  {
    lxb_css_parser_destroy(this->m_CssParserContext, true);
    this->m_CssParserContext = nullptr;
  }

  if (this->m_SelectorEngineContext != nullptr)
  {
    lxb_selectors_destroy(this->m_SelectorEngineContext, true);
    this->m_SelectorEngineContext = nullptr;
  }
}

bool LexborSelector::QuerySelect(const LexborDocument& Document, const std::string& CssQuery, LexborCollection& OutputCollection)
{
  if (this->m_CssParserContext == nullptr || this->m_SelectorEngineContext == nullptr || Document.GetNativeHandle() == nullptr)
  {
    return false;
  }

  // 1. Compile the C++ text string directly into a Lexbor CSS rule memory block
  lxb_css_selector_list_t* SelectorList = lxb_css_selectors_parse(
      this->m_CssParserContext,
      reinterpret_cast<const lxb_char_t*>(CssQuery.c_str()),
      CssQuery.length());

  if (SelectorList == nullptr)
  {
    std::cerr << "[ERROR] Lexbor failed to compile the CSS syntax string: " << CssQuery << std::endl;
    return false;
  }

  // 2. Clear out any old records hiding in the output collection bucket before searching
  lxb_dom_collection_clean(OutputCollection.GetNativeHandle());

  // 3. Run the compiled rules across the HTML document tree nodes
  lxb_status_t Status = lxb_selectors_find(
      this->m_SelectorEngineContext,
      lxb_dom_interface_node(Document.GetNativeHandle()),  // Cast document root down to raw node address
      SelectorList,
      LexborCollectionAppendCallback,  // Native callback function to fill the array
      OutputCollection.GetNativeHandle());

  // 4. CRITICAL: Clean up the compiled CSS query rule memory block to prevent a leak
  lxb_css_selector_list_destroy(SelectorList);

  return Status == LXB_STATUS_OK;
}
