// Handles file operations with the database.
#include "DataStore.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

DataStore::DataStore(const std::string& FilePath)
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

bool DataStore::SaveDraw(const DrawResult& Result)
{
  if (this->DrawExists(Result) == true)
  {
    std::cout << "[DEBUG] Skipping Draw Date: [" << Result.Date
              << "] already exists." << std::endl;
    return false;
  }

  std::ofstream File(this->FilePath, std::ios::app);

  if (!File.is_open())
  {
    std::cerr << "[ERROR] File to open or generate: " << this->FilePath << std::endl;
    return false;
  }

  File << Result.Date << ",";

  for (const unsigned int& Number : Result.Numbers)
  {
    File << Number << ",";
  }

  File << Result.BonusNumber << "\n";

  return true;
}

std::vector<DrawResult> DataStore::LoadAllDraws() const
{
  std::vector<DrawResult> RC;
  std::ifstream AllResults(this->FilePath);

  if (AllResults.is_open() == false)
  {
    std::cout << "[DEBUG] Results file not found! " << this->FilePath << std::endl;

    return RC;
  }

  std::string Line;

  while (std::getline(AllResults, Line))
  {
    DrawResult CurrentDrawResult;
    std::stringstream Stream(Line);
    std::string Field;

    std::getline(Stream, Field, ',');
    CurrentDrawResult.Date = Field;

    while (std::getline(Stream, Field, ','))
    {
      CurrentDrawResult.Numbers.push_back(std::stoul(Field));
    }

    CurrentDrawResult.BonusNumber = CurrentDrawResult.Numbers.back();
    CurrentDrawResult.Numbers.pop_back();
    RC.push_back(CurrentDrawResult);
  }

  return RC;
}

