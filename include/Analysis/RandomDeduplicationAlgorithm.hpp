#pragma once

#include "Analysis/IDeduplicationAlgorithm.hpp"
#include "TTree.h"
#include "TVector3.h"

/// @brief De-duplication algorithm class performing random track selection
///
/// For each event the algorithm selects a random set of tracks keeping track
/// of the clusters composing the selected tracks. In case a new track contains
/// clusters from the already selected tracks it is skipped
class RandomDeduplicationAlgorithm : public IDeduplicationAlgorithm {
 public:
  /// @brief Constructor
  ///
  /// @param selectionTree TTree pointer with track selection criteria branches
  /// @param trackHitsGlobal pointer to the global track hit branch address
  RandomDeduplicationAlgorithm(TTree* selectionTree,
                               std::vector<TVector3>* trackHitsGlobal);

  /// @brief remove shared clusters from a track sample
  ///
  /// @param startIdx index of the first event to process
  /// @param endIdx index of the last event to process
  /// @param eventRanges vector with TTree index collections for a given event
  ///
  /// @return set with indices of tracks with no overlapping clusters
  std::set<int> operator()(
      std::size_t startIdx, std::size_t endIdx,
      const std::vector<std::pair<std::uint32_t, std::uint32_t>>& eventRanges)
      const override;

 private:
  /// Tree with selection criteria branches
  TTree* m_selectionTree = nullptr;

  /// Measurement hits in the global frame
  std::vector<TVector3>* m_trackHitsGlobal = nullptr;
};
