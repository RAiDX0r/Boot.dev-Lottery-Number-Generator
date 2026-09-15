// Handles file operations with the database.
#include "DataStore.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

DataStore::DataStore()
{
}

bool DataStore::DrawExists(const GameDefinition& GameDef, const DrawResult& Result) const
{
  std::ifstream File(GetFilename(GameDef));

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

bool DataStore::SaveDraw(const GameDefinition& GameDef, const DrawResult& Result)
{
  std::string TargetFile = GetFilename(GameDef);

  if (DrawExists(GameDef, Result) == true)
  {
    std::cout << "[DEBUG] Skipping Draw Date: [" << Result.Date
              << "] already exists." << std::endl;
    return false;
  }

  std::ofstream File(TargetFile, std::ios::app);

  if (!File.is_open())
  {
    std::cerr << "[ERROR] File to open or generate: " << TargetFile << std::endl;
    return false;
  }

  File << Result.Date << ",";

  for (const unsigned int Number : Result.Numbers)
  {
    File << Number << ",";
  }

  File << Result.BonusNumber << "\n";

  return true;
}

std::vector<DrawResult> DataStore::LoadAllDraws(const GameDefinition& GameDef) const
{
  std::vector<DrawResult> RC;
  std::string TargetFile = GetFilename(GameDef);
  std::ifstream AllResults(TargetFile);

  if (AllResults.is_open() == false)
  {
    std::cout << "[DEBUG] Results file not found! " << TargetFile << std::endl;

    return RC;
  }

  std::string Line;

  while (std::getline(AllResults, Line))
  {
    DrawResult CurrentDrawResult;
    std::stringstream Stream(Line);
    std::string Field;

    CurrentDrawResult.GameType = GameDef.Id;
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

std::string DataStore::GetFilename(const GameDefinition& GameDef) const
{
  if (GameDef.Id.empty() == true)
  {
    throw std::invalid_argument("Game Definition ID cannot be empty.");
  }

  return GameDef.Id + "_data.csv";
}
