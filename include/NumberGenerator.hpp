#pragma once

#include <map>
#include <vector>

#include "Types.hpp"

class NumberGenerator
{
 public:
  std::vector<std::vector<unsigned int>> GenerateDrawPicks(const std::map<unsigned int, unsigned int>& FrequencyMap, const std::map<unsigned int, unsigned int>& SkipMap) const;
};
