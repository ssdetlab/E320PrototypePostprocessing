#pragma once

#include <cstdint>
#include <set>
#include <vector>

/// @brief Interface class for de-duplication algorithms calls
///
/// Interface for the shared cluster removal algorithms. Access
/// of the algorithms to the track data files and structures is
/// left user-defined and the interface is kept minimal
class IDeduplicationAlgorithm {
 public:
  /// @brief remove shared clusters from a track sample
  ///
  /// @param startIdx index of the first event to process
  /// @param endIdx index of the last event to process
  /// @param eventRanges vector with TTree index collections for a given event
  ///
  /// @return set with indices of tracks with no overlapping clusters
  virtual std::set<int> operator()(
      std::size_t startIdx, std::size_t endIdx,
      const std::vector<std::pair<std::uint32_t, std::uint32_t>> &eventRanges)
      const = 0;
};
