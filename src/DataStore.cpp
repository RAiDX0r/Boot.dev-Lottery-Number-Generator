// Handles file operations with the database.
#include "DataStore.hpp"
#include <fstream>
#include <sstream>

DataStore::DataStore(const std::string &FilePath)
{
  this->FilePath = FilePath;
}

bool DataStore::DrawExists(const DrawResult& Result) const
{
  std::ifstream File(FilePath);

  if (!File.is_open())
  {
    return false;
  }

  std::string Line;

  while (std::getline(File, Line))
  {
    if (Line.find(Result.Date) != std::string::npos)
    {
      return true;
    }
  }

  return false;
}
