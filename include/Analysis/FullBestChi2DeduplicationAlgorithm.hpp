#pragma once

#include "Analysis/IDeduplicationAlgorithm.hpp"
#include "TTree.h"
#include "TVector3.h"

/// @brief De-duplication algorithm class performing full best chi2 selection
///
/// For each event the algorithm iterates over the event clusters and for each
/// cluster keeps the best chi2 track passing through it. A track is selected
/// if it is the best chi2 track for every cluster composing it
class FullBestChi2DeduplicationAlgorithm : public IDeduplicationAlgorithm {
 public:
  /// @brief Constructor
  ///
  /// @param selectionTree TTree pointer with track selection criteria branches
  /// @param chi2Smoothed pointer to the smoothed chi2 branch address
  /// /// @param trackHitsGlobal pointer to the global track hit branch address
  FullBestChi2DeduplicationAlgorithm(TTree* selectionTree, double* chi2Smoothed,
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

  /// Chi2 of the track
  /// with respect ot the
  /// measurement
  double* m_chi2Smoothed = nullptr;

  /// Measurement hits in the global frame
  std::vector<TVector3>* m_trackHitsGlobal = nullptr;
};
