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

bool DataStore::DrawExists(LotteryGame Game, const DrawResult& Result) const
{
  std::ifstream File(GetFilename(Game));

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

bool DataStore::SaveDraw(LotteryGame Game, const DrawResult& Result)
{
  std::string TargetFile = GetFilename(Game);

  if (DrawExists(Game, Result) == true)
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

  for (const unsigned int& Number : Result.Numbers)
  {
    File << Number << ",";
  }

  File << Result.BonusNumber << "\n";

  return true;
}

std::vector<DrawResult> DataStore::LoadAllDraws(LotteryGame Game) const
{
  std::vector<DrawResult> RC;
  std::string TargetFile = GetFilename(Game);
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

    CurrentDrawResult.GameType = Game;
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

std::string DataStore::GetFilename(LotteryGame Game) const
{
  switch (Game)
  {
    case LotteryGame::LottoMax:
    {
      return "lotto_max_data.csv";
    }
    case LotteryGame::Lotto649:
    {
      return "lotto_649_data.csv";
    }
    default:
    {
      throw std::invalid_argument("Error: Unknown lottery game type encountered.");
    }
  }
}
