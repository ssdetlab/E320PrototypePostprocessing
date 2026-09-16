#include "Analysis/FullBestChi2DeduplicationAlgorithm.hpp"

#include <iostream>

#include "detail/TVector3Hash.hpp"
#include "detail/TrackHandle.hpp"

FullBestChi2DeduplicationAlgorithm::FullBestChi2DeduplicationAlgorithm(
    TTree *selectionTree, double *chi2Smoothed,
    std::vector<TVector3> *trackHitsGlobal)
    : m_selectionTree(selectionTree),
      m_chi2Smoothed(chi2Smoothed),
      m_trackHitsGlobal(trackHitsGlobal) {}

std::set<int> FullBestChi2DeduplicationAlgorithm::operator()(
    std::size_t startIdx, std::size_t endIdx,
    const std::vector<std::pair<std::uint32_t, std::uint32_t>> &eventRanges)
    const {
  std::set<int> uniqueTrackIdxs;
  std::size_t nRanges = eventRanges.size();
  for (std::size_t i = startIdx; i < endIdx; i++) {
    auto [eventStartIdx, eventEndIdx] = eventRanges.at(i);

    // Go over the hits and indetify the best chi2
    // track passing through each of them
    std::unordered_set<int> badIdxs;
    badIdxs.reserve(eventEndIdx - eventStartIdx);
    std::unordered_map<TVector3, TrackHandle, TVector3Hash, TVector3Eq>
        clusterMap;
    clusterMap.reserve((eventEndIdx - eventStartIdx) * 5);
    for (int j = eventStartIdx; j < eventEndIdx; j++) {
      m_selectionTree->GetEntry(j);
      for (const auto &hit : *m_trackHitsGlobal) {
        auto &mapEntry = clusterMap[hit];
        if (mapEntry.chi2 > *m_chi2Smoothed) {
          badIdxs.insert(mapEntry.treeIdx);
          mapEntry = {j, *m_chi2Smoothed};
        } else {
          badIdxs.insert(j);
        }
      }
    }

    // Get the unique track indices
    for (const auto &[hit, track] : clusterMap) {
      const auto &[idx, chi2, magId] = track;
      if (badIdxs.contains(idx)) {
        continue;
      }
      uniqueTrackIdxs.insert(idx);
    }
    if (i % 1000 == 0) {
      std::cout << i << "/" << nRanges << "\n";
    }
  }
  return std::move(uniqueTrackIdxs);
}
