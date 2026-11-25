#include "Analysis/RandomDeduplicationAlgorithm.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "TVector3.h"
#include "detail/TVector3Hash.hpp"

RandomDeduplicationAlgorithm::RandomDeduplicationAlgorithm(
    TTree *selectionTree, std::vector<TVector3> *trackHitsGlobal,
    std::vector<std::size_t> *geometryIds, std::size_t ipGeoId,
    std::uint64_t seed)
    : m_selectionTree(selectionTree),
      m_trackHitsGlobal(trackHitsGlobal),
      m_geometryIds(geometryIds),
      m_ipGeoId(ipGeoId),
      m_seed(seed) {}

std::set<int> RandomDeduplicationAlgorithm::operator()(
    std::size_t startIdx, std::size_t endIdx,
    const std::vector<std::pair<std::uint32_t, std::uint32_t>> &eventRanges)
    const {
  std::mt19937_64 rng(m_seed);

  std::set<int> uniqueTrackIdxs;
  std::size_t nRanges = eventRanges.size();
  for (std::size_t i = startIdx; i < endIdx; i++) {
    auto [eventStartIdx, eventEndIdx] = eventRanges.at(i);

    std::unordered_map<std::size_t, std::vector<TVector3>> tracks;
    tracks.reserve(eventEndIdx - eventStartIdx);
    for (std::size_t idx = eventStartIdx; idx < eventEndIdx; idx++) {
      m_selectionTree->GetEntry(idx);

      // Skip the IP measurement shared by all the tracks
      std::vector<TVector3> trackHits;
      trackHits.reserve(m_trackHitsGlobal->size());
      for (std::size_t k = 0; k < m_trackHitsGlobal->size(); k++) {
        if (m_geometryIds->at(k) == m_ipGeoId) {
          continue;
        }
        trackHits.push_back(m_trackHitsGlobal->at(k));
      }
      tracks.insert({idx, trackHits});
    }

    std::vector<int> trackIdxs;
    trackIdxs.reserve(eventEndIdx - eventStartIdx);
    for (int j = eventStartIdx; j < static_cast<int>(eventEndIdx); ++j) {
      trackIdxs.push_back(j);
    }
    std::shuffle(trackIdxs.begin(), trackIdxs.end(), rng);

    std::unordered_set<TVector3, TVector3Hash, TVector3Eq> usedClusters;
    for (const int j : trackIdxs) {
      auto trackHitsGlobal = tracks.at(j);

      bool hasUsed = false;
      for (const auto &hit : trackHitsGlobal) {
        if (usedClusters.contains(hit)) {
          hasUsed = true;
          break;
        }
      }
      if (hasUsed) {
        continue;
      }

      uniqueTrackIdxs.insert(j);
      for (const auto &hit : trackHitsGlobal) {
        usedClusters.insert(hit);
      }
    }
    if (i % 1000 == 0) {
      std::cout << i << "/" << nRanges << "\n";
    }
  }
  return uniqueTrackIdxs;
}
