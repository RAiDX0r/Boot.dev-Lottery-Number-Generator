#pragma once

#include <lexbor/html/html.h>  // Pulls in Lexbor's native C core headers

#include <string>

/**
 * @brief RAII safe memory management wrapper container around a raw Lexbor HTML document tree pointer.
 */
class LexborDocument
{
 public:
  /**
   * @brief Constructs a managed document instance and instantly parses a raw HTML text string buffer.
   * @param RawHtml The raw web markup string data to process.
   */
  LexborDocument(const std::string& RawHtml);

  /**
   * @brief Destructor. Automatically triggers low-level C memory scrub routines to prevent leaks.
   */
  ~LexborDocument();

  /**
   * @brief Accesses the internal raw Lexbor pointer directly.
   * @return A read-only pointer to the native low-level lxb_html_document_t structural record.
   */
  lxb_html_document_t* GetNativeHandle() const;

 private:
  // The raw underlying C pointer hook tracking our memory space
  lxb_html_document_t* m_DocumentContext;
};
