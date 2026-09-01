#include <iostream>
#include <string>
#include <vector>

#include "DataStore.hpp"
#include "NetworkClient.hpp"
#include "Types.hpp"

int main()
{
  std::cout << "--- Modern Stateless DataStore Test Bench ---" << std::endl;

  // 1. Initialize our stateless database management machine
  DataStore Storage;

  // 2. Manufacture a fake mock draw dataset to verify local file track routing
  DrawResult MockLottoMaxDraw;
  MockLottoMaxDraw.GameType = LotteryGame::LottoMax;
  MockLottoMaxDraw.Date = "2026-08-28";
  MockLottoMaxDraw.Numbers = {1, 7, 8, 10, 41, 42, 48};
  MockLottoMaxDraw.BonusNumber = 29;

  DrawResult MockLotto649Draw;
  MockLotto649Draw.GameType = LotteryGame::Lotto649;
  MockLotto649Draw.Date = "2026-08-29";
  MockLotto649Draw.Numbers = {4, 15, 23, 38, 42, 44};
  MockLotto649Draw.BonusNumber = 11;

  // 3. Test execution for Lotto Max tracking paths
  std::cout << "\n[TEST] Writing Mock Lotto Max record..." << std::endl;
  bool MaxSaved = Storage.SaveDraw(LotteryGame::LottoMax, MockLottoMaxDraw);

  if (MaxSaved == true)
  {
    std::cout << "-> SUCCESS: Lotto Max record logged to disk." << std::endl;
  }
  else
  {
    std::cout << "-> NOTICE: Lotto Max file skipped (Duplicate record detected)." << std::endl;
  }

  // 4. Test execution for Lotto 6/49 tracking paths
  std::cout << "\n[TEST] Writing Mock Lotto 6/49 record..." << std::endl;
  bool Lotto649Saved = Storage.SaveDraw(LotteryGame::Lotto649, MockLotto649Draw);

  if (Lotto649Saved == true)
  {
    std::cout << "-> SUCCESS: Lotto 6/49 record logged to disk." << std::endl;
  }
  else
  {
    std::cout << "-> NOTICE: Lotto 6/49 file skipped (Duplicate record detected)." << std::endl;
  }

  // 5. Test data retrieval from separate file environments back into RAM
  std::cout << "\n[TEST] Verifying in-memory data reload tracking..." << std::endl;
  
  std::vector<DrawResult> LoadedMaxDraws = Storage.LoadAllDraws(LotteryGame::LottoMax);
  std::cout << "Total Lotto Max entries found in file: " << LoadedMaxDraws.size() << std::endl;

  std::vector<DrawResult> Loaded649Draws = Storage.LoadAllDraws(LotteryGame::Lotto649);
  std::cout << "Total Lotto 6/49 entries found in file: " << Loaded649Draws.size() << std::endl;

  std::cout << "\n---------------------------------------------" << std::endl;
  std::cout << "--- Test Bench Run Successfully Completed ---" << std::endl;

  return 0;
}
