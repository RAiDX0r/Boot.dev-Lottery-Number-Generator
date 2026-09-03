#include "LexborSelector.hpp"

#include <iostream>

static lxb_status_t LexborCollectionAppendCallback(lxb_dom_node_t* Node, lxb_css_selector_specificity_t Spec, void* Ctx)
{
  lxb_dom_collection_t* Collection = reinterpret_cast<lxb_dom_collection_t*>(Ctx);
  return lxb_dom_collection_append(Collection, lxb_dom_interface_element(Node));
}

// Keep constructors and destructors empty because memory lifecycle management
// is now handled completely within the QuerySelect transaction boundary!
LexborSelector::LexborSelector()
{
  this->m_CssParserContext = nullptr;
  this->m_SelectorEngineContext = nullptr;
}

LexborSelector::~LexborSelector()
{
}

bool LexborSelector::QuerySelect(const LexborDocument& Document, const std::string& CssQuery, LexborCollection& OutputCollection)
{
  if (Document.GetNativeHandle() == nullptr)
  {
    return false;
  }

  // 1. Create a pristine, isolated parser and selector engine context on the fly
  lxb_css_parser_t* ParserContext = lxb_css_parser_create();
  lxb_selectors_t* SelectorContext = lxb_selectors_create();

  if (ParserContext == nullptr || SelectorContext == nullptr)
  {
    if (ParserContext != nullptr) lxb_css_parser_destroy(ParserContext, true);
    if (SelectorContext != nullptr) lxb_selectors_destroy(SelectorContext, true);
    return false;
  }

  lxb_css_parser_init(ParserContext, nullptr);

  // 2. Compile the compound CSS target path string cleanly
  lxb_css_selector_list_t* SelectorList = lxb_css_selectors_parse(
      ParserContext,
      reinterpret_cast<const lxb_char_t*>(CssQuery.c_str()),
      CssQuery.length());

  if (SelectorList == nullptr)
  {
    std::cerr << "[ERROR] Lexbor failed to compile the CSS syntax string: " << CssQuery << std::endl;
    lxb_css_parser_destroy(ParserContext, true);
    lxb_selectors_destroy(SelectorContext, true);
    return false;
  }

  // 3. Clear the output collection target array tray
  lxb_dom_collection_clean(OutputCollection.GetNativeHandle());

  // 4. Run the match algorithm across the DOM tree node references
  lxb_status_t Status = lxb_selectors_find(
      SelectorContext,
      lxb_dom_interface_node(Document.GetNativeHandle()),
      SelectorList,
      LexborCollectionAppendCallback,
      OutputCollection.GetNativeHandle());

  // 5. CRITICAL ACTION: Free all localized tracking structures completely to prevent memory leaks
  lxb_css_selector_list_destroy(SelectorList);
  lxb_css_parser_destroy(ParserContext, true);
  lxb_selectors_destroy(SelectorContext, true);

  return Status == LXB_STATUS_OK;
}
