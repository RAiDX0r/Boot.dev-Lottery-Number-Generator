#include "LexborCollection.hpp"

LexborCollection::LexborCollection()
{
  // Allocate a fresh DOM collection container block in system memory
  this->m_CollectionContext = lxb_dom_collection_create(nullptr);
}

LexborCollection::~LexborCollection()
{
  if (this->m_CollectionContext != nullptr)
  {
    // Clean up and reclaim 100% of the allocated memory space automatically
    lxb_dom_collection_destroy(this->m_CollectionContext, true);
    this->m_CollectionContext = nullptr;
  }
}

size_t LexborCollection::GetSize() const
{
  if (this->m_CollectionContext == nullptr)
  {
    return 0;
  }

  // Retrieve the total number of matched items from Lexbor's internal structure
  return lxb_dom_collection_length(this->m_CollectionContext);
}

lxb_dom_element_t* LexborCollection::GetElementAt(size_t Index) const
{
  if (this->m_CollectionContext == nullptr || Index >= this->GetSize())
  {
    return nullptr;
  }

  // Fetch the specific element pointer at the requested position array index
  return lxb_dom_collection_element(this->m_CollectionContext, Index);
}

lxb_dom_collection_t* LexborCollection::GetNativeHandle() const
{
  return this->m_CollectionContext;
}
