#include "LexborDocument.hpp"

#include <iostream>

LexborDocument::LexborDocument(const std::string& RawHtml)
{
  // 1. Allocate a fresh, uninitialized memory block for our document tree container
  this->m_DocumentContext = lxb_html_document_create();

  if (this->m_DocumentContext == nullptr)
  {
    std::cerr << "[CRITICAL] Lexbor failed to allocate heap memory block for document framework." << std::endl;
    return;
  }

  // 2. Parse the raw HTML text string bytes directly into the allocated memory structure
  lxb_status_t Status = lxb_html_document_parse(
      this->m_DocumentContext,
      reinterpret_cast<const lxb_char_t*>(RawHtml.c_str()),  // Cast from signed char* to unsigned lxb_char_t*
      RawHtml.length());

  if (Status != LXB_STATUS_OK)
  {
    std::cerr << "[ERROR] Lexbor encountered an internal parsing error anomaly. Status Code: " << Status << std::endl;
  }
}

LexborDocument::~LexborDocument()
{
  if (this->m_DocumentContext != nullptr)
  {
    // 3. The RAII core: Automatically vaporize and reclaim 100% of the allocated heap space
    lxb_html_document_destroy(this->m_DocumentContext);
    this->m_DocumentContext = nullptr;
  }
}

lxb_html_document_t* LexborDocument::GetNativeHandle() const
{
  return this->m_DocumentContext;
}
