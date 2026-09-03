#include "LexborSelector.hpp"
#include <iostream>


#define LEXBOR_SELECTOR_DEBUG 0

static lxb_status_t LexborCollectionAppendCallback(lxb_dom_node_t* Node, lxb_css_selector_specificity_t Spec, void* Ctx)
{
  lxb_dom_collection_t* Collection = reinterpret_cast<lxb_dom_collection_t*>(Ctx);
  return lxb_dom_collection_append(Collection, lxb_dom_interface_element(Node));
}

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

#if LEXBOR_SELECTOR_DEBUG
  std::cout << "      [SELECTOR TRACE] Compiling Query Path: '" << CssQuery << "'" << std::endl;
#endif

  // 1. Create a pristine, isolated parser context on the fly
  lxb_css_parser_t* ParserContext = lxb_css_parser_create();
  if (ParserContext == nullptr)
  {
    return false;
  }
  lxb_css_parser_init(ParserContext, nullptr);

  // 2. Compile the target CSS selector string using the verified function signature
#if LEXBOR_SELECTOR_DEBUG
  std::cout << "      [SELECTOR TRACE] Invoking verified string parser compilation block..." << std::endl;
#endif
  lxb_css_selector_list_t* SelectorList = lxb_css_selectors_parse(
      ParserContext,
      reinterpret_cast<const lxb_char_t*>(CssQuery.c_str()),
      CssQuery.length());

  if (SelectorList == nullptr)
  {
    std::cerr << "[ERROR] Lexbor failed to compile the CSS syntax string: " << CssQuery << std::endl;
    lxb_css_parser_destroy(ParserContext, true);
    return false;
  }

  // 3. Clear the target output collection tray
  lxb_dom_collection_clean(OutputCollection.GetNativeHandle());

  // 4. Create and initialize the separate high-level DOM selector engine match context
#if LEXBOR_SELECTOR_DEBUG
  std::cout << "      [SELECTOR TRACE] Initializing high-level DOM selectors engine context..." << std::endl;
#endif
  lxb_selectors_t* SelectorContext = lxb_selectors_create();
  if (SelectorContext == nullptr)
  {
    lxb_css_selector_list_destroy(SelectorList);
    lxb_css_parser_destroy(ParserContext, true);
    return false;
  }
  
  // 💡 FIXED: Natively initialize the cache tables according to your verified header spec!
  lxb_selectors_init(SelectorContext);

  // 5. Run the high-performance search algorithm across the DOM tree
#if LEXBOR_SELECTOR_DEBUG
  std::cout << "      [SELECTOR TRACE] Executing lxb_selectors_find tree search..." << std::endl;
#endif
  lxb_status_t Status = lxb_selectors_find(
      SelectorContext,
      lxb_dom_interface_node(Document.GetNativeHandle()),
      SelectorList,
      LexborCollectionAppendCallback,
      OutputCollection.GetNativeHandle());

#if LEXBOR_SELECTOR_DEBUG
  std::cout << "      [SELECTOR TRACE] Traversal finish. Found Match Count: " << OutputCollection.GetSize() << std::endl;
#endif

  // 6. Free all temporary compilation structures and engine contexts cleanly
  lxb_css_selector_list_destroy(SelectorList);
  lxb_css_parser_destroy(ParserContext, true);
  lxb_selectors_destroy(SelectorContext, true);

  return Status == LXB_STATUS_OK;
}
