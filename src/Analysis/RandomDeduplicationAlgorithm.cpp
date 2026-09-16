#include "Analysis/RandomDeduplicationAlgorithm.hpp"

#include <chrono>
#include <iostream>
#include <random>

#include "TVector3.h"
#include "detail/TVector3Hash.hpp"

RandomDeduplicationAlgorithm::RandomDeduplicationAlgorithm(
    TTree *selectionTree, std::vector<TVector3> *trackHitsGlobal)
    : m_selectionTree(selectionTree), m_trackHitsGlobal(trackHitsGlobal) {}

std::set<int> RandomDeduplicationAlgorithm::operator()(
    std::size_t startIdx, std::size_t endIdx,
    const std::vector<std::pair<std::uint32_t, std::uint32_t>> &eventRanges)
    const {
  std::uniform_int_distribution<> uniform(0, 1);
  std::mt19937_64 rng;
  rng.seed(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());

  std::set<int> uniqueTrackIdxs;
  std::size_t nRanges = eventRanges.size();
  for (std::size_t i = startIdx; i < endIdx; i++) {
    auto [eventStartIdx, eventEndIdx] = eventRanges.at(i);

    std::vector<int> trackIdxs;
    trackIdxs.reserve(eventEndIdx - eventStartIdx);
    for (int j = eventStartIdx; j < eventEndIdx; ++j) {
      trackIdxs.push_back(j);
    }
    std::shuffle(trackIdxs.begin(), trackIdxs.end(), rng);

    std::unordered_set<TVector3, TVector3Hash, TVector3Eq> usedClusters;
    for (const int j : trackIdxs) {
      m_selectionTree->GetEntry(j);

      bool hasUsed = false;
      for (const auto &hit : *m_trackHitsGlobal) {
        if (usedClusters.contains(hit)) {
          hasUsed = true;
          break;
        }
      }
      if (hasUsed) {
        continue;
      }

      uniqueTrackIdxs.insert(j);
      for (const auto &hit : *m_trackHitsGlobal) {
        usedClusters.insert(hit);
      }
    }
    if (i % 1000 == 0) {
      std::cout << i << "/" << nRanges << "\n";
    }
  }
  return std::move(uniqueTrackIdxs);
}
