#include "Analysis/FullBestChi2DeduplicationAlgorithm.hpp"

#include <iostream>

#include "detail/TVector3Hash.hpp"
#include "detail/TrackHandle.hpp"

FullBestChi2DeduplicationAlgorithm::FullBestChi2DeduplicationAlgorithm(
    TTree *selectionTree, double *chi2Smoothed,
    std::vector<TVector3> *trackHitsGlobal,
    std::vector<std::size_t> *geometryIds, std::size_t ipGeoId)
    : m_selectionTree(selectionTree),
      m_chi2Smoothed(chi2Smoothed),
      m_trackHitsGlobal(trackHitsGlobal),
      m_geometryIds(geometryIds),
      m_ipGeoId(ipGeoId) {}

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
      for (std::size_t k = 0; k < m_trackHitsGlobal->size(); k++) {
        // Skip the IP measurement shared by all the tracks
        if (m_geometryIds->at(k) == m_ipGeoId) {
          continue;
        }
        auto &mapEntry = clusterMap[m_trackHitsGlobal->at(k)];
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
