#pragma once

#include <string>
#include <vector>

#include "Types.hpp"

/**
 * @brief Manages local file-system storage for lottery drawing records.
 *
 * This class handles writing drawing metrics out to flat CSV files and reading
 * historical logs back into system RAM for metrics calculation workflows.
 */
class DataStore
{
 public:
  /**
   * @brief Constructs a stateless DataStore database management utility.
   */
  DataStore();

  /**
   * @brief Scans the internal CSV database file to check for a duplicate draw
   * date.
   *
   * @param Game The target lottery game context.
   * @param Result The drawing package containing the target verification date.
   * @return true if the drawing date is already logged, false if it is a new
   * entry.
   */
  bool DrawExists(LotteryGame Game, const DrawResult& Result) const;

  /**
   * @brief Appends a new lottery drawing to our local database file.
   *
   * This method automatically runs a duplicate validation sweep. If the draw
   * date already exists inside the file, the operation is safely skipped.
   *
   * @param Game The target lottery game context.
   * @param Result The clean lottery drawing struct package to save.
   * @return true if the row was successfully appended, false if it was a
   * duplicate.
   */
  bool SaveDraw(LotteryGame Game, const DrawResult& Result);

  /**
   * @brief Reads all historical drawing rows out of the file back into system
   * RAM.
   *
   * @param Game The target lottery game context.
   * @return A standard vector collection holding every logged DrawResult
   * struct.
   */
  std::vector<DrawResult> LoadAllDraws(LotteryGame Game) const;

 private:
  /**
   * @brief Get the Filename based on the type of lottery game.
   *
   * @param Game Lottery game type.
   * @return std::string String representing the filename for the game type.
   */
  std::string GetFilename(LotteryGame Game) const;
};
