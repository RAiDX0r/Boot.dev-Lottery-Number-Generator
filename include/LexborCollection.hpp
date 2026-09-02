#pragma once

#include <lexbor/dom/dom.h>

/**
 * Safe Storage Tray
 * In pure C, Lexbor outputs data arrays that can easily leak memory if you forget to clean them up.
 * This class acts as a secure C++ structural tray. It tracks your matched elements and guarantees
 * that when your loops finish executing, 100% of that memory space is auto-vaporized cleanly by
 * its destructor.
 */

/**
 * @brief RAII safe memory management wrapper container around a raw Lexbor DOM element collection.
 */
class LexborCollection
{
 public:
  /**
   * @brief Constructs an empty managed collection instance.
   */
  LexborCollection();

  /**
   * @brief Destructor. Safely frees the internal Lexbor collection memory buffer to prevent memory leaks.
   */
  ~LexborCollection();

  /**
   * @brief Gets the total number of matched HTML elements stored in this collection.
   * @return The element count as a standard size tracking integer.
   */
  size_t GetSize() const;

  /**
   * @brief Fetches a specific HTML node out of the collection array by index.
   * @param Index The numerical position to look up.
   * @return A raw pointer to the native Lexbor element node structure, or nullptr if out of bounds.
   */
  lxb_dom_element_t* GetElementAt(size_t Index) const;

  /**
   * @brief Accesses the internal raw Lexbor pointer directly for writing search results.
   * @return A pointer to the underlying lxb_dom_collection_t struct pointer handle.
   */
  lxb_dom_collection_t* GetNativeHandle() const;

 private:
  // The raw underlying C pointer hook tracking our collection allocation
  lxb_dom_collection_t* m_CollectionContext;
};
