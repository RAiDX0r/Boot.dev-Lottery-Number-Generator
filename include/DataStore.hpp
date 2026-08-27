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
  /// The local relative or absolute system path to our database file.
  std::string FilePath = "";

  /**
   * @brief Scans the internal CSV database file to check for a duplicate draw
   * date.
   *
   * @param Result The drawing package containing the target verification date.
   * @return true if the drawing date is already logged, false if it is a new
   * entry.
   */
  bool DrawExists(const DrawResult& Result) const;

 public:
  /**
   * @brief Constructs a new DataStore machine linked to a specific file target.
   *
   * @param FilePath The system path to use for saving and loading operations.
   */
  explicit DataStore(const std::string& FilePath);

  /**
   * @brief Appends a new lottery drawing to our local database file.
   *
   * This method automatically runs a duplicate validation sweep. If the draw
   * date already exists inside the file, the operation is safely skipped.
   *
   * @param Result The clean lottery drawing struct package to save.
   * @return true if the row was successfully appended, false if it was a
   * duplicate.
   */
  bool SaveDraw(const DrawResult& Result);

  /**
   * @brief Reads all historical drawing rows out of the file back into system
   * RAM.
   *
   * @return A standard vector collection holding every logged DrawResult
   * struct.
   */
  std::vector<DrawResult> LoadAllDraws() const;
};
