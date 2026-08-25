#pragma once

#include <string>
#include <vector>
#include "Types.hpp"

class DataStore
{
  std::string FilePath = "";
  
  bool DrawExists(const DrawResult& Result) const;

public:
  explicit DataStore(const std::string& FilePath);
  bool SaveDraw(const DrawResult& Result);
  std::vector<DrawResult> LoadAllDraws() const;
};
