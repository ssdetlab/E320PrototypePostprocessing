#include "Analysis/Postprocessor.hpp"

#include <cmath>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "detail/TVector3Hash.hpp"

std::vector<std::string> Postprocessor::collectDataPaths(
    const Options &opt) const {
  std::cout << "Collecting data paths from\n" << opt.inDataDir << "\n";

  // Collect paths
  std::vector<std::string> paths;
  std::size_t idx = 0;
  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(opt.inDataDir)) {
    if (!entry.is_regular_file() || entry.path().extension() != ".root") {
      continue;
    }
    paths.push_back(entry.path());
    idx++;
    if (idx > 2) {
      // break;
    }
  }
  std::sort(paths.begin(), paths.end());

  // Print debug info
  std::cout << "---------------------------------------------\n";
  std::cout << "All paths:\n";
  for (const auto &path : paths) {
    std::cout << path << "\n";
  }
  std::cout << "---------------------------------------------\n";
  return paths;
};

Postprocessor::FileHandle Postprocessor::getInFileHandle(
    const std::string &treeName, const std::vector<std::string> &paths) {
  TFile *file = nullptr;
  TTree *tree = nullptr;
  TChain *chainOwner = new TChain(treeName.c_str(), treeName.c_str());
  if (paths.size() == 1) {
    file = new TFile(paths.at(0).c_str());
    tree = file->Get<TTree>(treeName.c_str());
  } else {
    chainOwner = new TChain(treeName.c_str());
    for (const auto &path : paths) {
      chainOwner->Add(path.c_str());
    }
    tree = dynamic_cast<TTree *>(chainOwner);
  }

  // EPICS meta
  tree->SetBranchAddress("epicsParity", &m_epicsParity);
  tree->SetBranchAddress("epicsPulseId", &m_epicsPulseId);
  tree->SetBranchAddress("epicsDAQNumber", &m_epicsDAQNumber);

  // Measurement hits
  tree->SetBranchAddress("trackHitsGlobal", &m_trackHitsGlobal);
  tree->SetBranchAddress("trackHitsLocal", &m_trackHitsLocal);

  // Covariances of the track hits
  tree->SetBranchAddress("trackHitsCovs", &m_trackHitsCovs);

  // Geometry ids of the track hits
  tree->SetBranchAddress("geometryIds", &m_geometryIds);

  // KF predicted track hits
  tree->SetBranchAddress("predictedTrackHitsGlobal",
                         &m_predictedTrackHitsGlobal);
  tree->SetBranchAddress("filteredTrackHitsGlobal", &m_filteredTrackHitsGlobal);
  tree->SetBranchAddress("smoothedTrackHitsGlobal", &m_smoothedTrackHitsGlobal);

  tree->SetBranchAddress("predictedTrackHitsLocal", &m_predictedTrackHitsLocal);
  tree->SetBranchAddress("filteredTrackHitsLocal", &m_filteredTrackHitsLocal);
  tree->SetBranchAddress("smoothedTrackHitsLocal", &m_smoothedTrackHitsLocal);

  // KF residuals with respect to the measurements
  tree->SetBranchAddress("predictedResiduals", &m_predictedResiduals);
  tree->SetBranchAddress("filteredResiduals", &m_filteredResiduals);
  tree->SetBranchAddress("smoothedResiduals", &m_smoothedResiduals);

  // KF pulls with respect to the measurements
  tree->SetBranchAddress("predictedPulls", &m_predictedPulls);
  tree->SetBranchAddress("filteredPulls", &m_filteredPulls);
  tree->SetBranchAddress("smoothedPulls", &m_smoothedPulls);

  // Initial guess of the momentum at the IP
  tree->SetBranchAddress("originMomentumGuess", &m_originMomentumGuess);
  tree->SetBranchAddress("vertexGuess", &m_vertexGuess);

  /// Guessed bound track parameters
  tree->SetBranchAddress("boundTrackParametersGuess",
                         &m_boundTrackParametersGuess);
  tree->SetBranchAddress("boundTrackCovGuess", &m_boundTrackCovGuess);

  /// KF predicted bound track parameters
  tree->SetBranchAddress("boundTrackParametersEst", &m_boundTrackParametersEst);
  tree->SetBranchAddress("boundTrackCovEst", &m_boundTrackCovEst);

  /// Initial guess of the momentum at the IP
  tree->SetBranchAddress("originMomentumGuess", &m_originMomentumGuess);

  /// Initial guess of the vertex at the IP
  tree->SetBranchAddress("vertexGuess", &m_vertexGuess);

  /// KF predicted momentum at the IP
  tree->SetBranchAddress("originMomentumEst", &m_originMomentumEst);

  /// KF predicted vertex at the IP
  tree->SetBranchAddress("vertexEst", &m_vertexEst);

  // Chi2 and ndf of the fitted track
  tree->SetBranchAddress("chi2Predicted", &m_chi2Predicted);
  tree->SetBranchAddress("chi2Filtered", &m_chi2Filtered);
  tree->SetBranchAddress("chi2Smoothed", &m_chi2Smoothed);
  tree->SetBranchAddress("ndf", &m_ndf);

  // Track ID
  tree->SetBranchAddress("trackId", &m_trackId);

  // Event ID
  tree->SetBranchAddress("eventId", &m_eventId);

  // PDG ID
  tree->SetBranchAddress("pdgId", &m_pdgId);

  // Charge
  tree->SetBranchAddress("charge", &m_charge);

  return {file, chainOwner, tree};
}

std::vector<std::pair<std::uint32_t, std::uint32_t>>
Postprocessor::getEventRanges(TTree *tree) const {
  std::cout << "Getting event ranges\n";
  std::vector<std::pair<std::uint32_t, std::uint32_t>> eventRanges;

  tree->SetBranchStatus("*", false);
  tree->SetBranchStatus("eventId", true);
  tree->GetEntry(0);
  std::size_t currentEventId = m_eventId;
  std::size_t currentIdx = 0;
  std::size_t nEntries = tree->GetEntries();
  for (std::size_t i = 1; i < nEntries; i++) {
    tree->GetEntry(i);
    if (m_eventId != currentEventId || i == nEntries - 1) {
      if (i != nEntries - 1) {
        eventRanges.push_back({currentIdx, i});
        currentIdx = i;
        currentEventId = m_eventId;
      } else {
        eventRanges.push_back({currentIdx, i + 1});
        currentIdx = i;
        currentEventId = m_eventId;
      }
    }
  }
  // throw std::runtime_error("ERR");
  tree->SetBranchStatus("*", true);

  std::cout << "Total ranges found " << eventRanges.size() << "\n";
  std::cout << "\n\n\n\n";
  return eventRanges;
}

void Postprocessor::processFiles(const Options &opt) {
  auto paths = collectDataPaths(opt);

  // Get data tree
  auto [inFile, inChain, inTree] = getInFileHandle(opt.inDataTreeName, paths);

  // Initilize output file
  TFile *outFile = new TFile(opt.outDataPath.c_str(), "RECREATE");
  TTree *outTree = nullptr;
  outTree = inTree->CloneTree(0);
  inTree->CopyAddresses(outTree);

  // Get event ranges
  auto eventRanges = getEventRanges(inTree);

  // Process events
  std::size_t nRanges = eventRanges.size();
  std::size_t startIdx = opt.skip;
  std::size_t endIdx = std::min(opt.skip + opt.events, nRanges);
  for (std::size_t i = startIdx; i < endIdx; i++) {
    auto [eventStartIdx, eventEndIdx] = eventRanges.at(i);

    inTree->SetBranchStatus("*", false);
    inTree->SetBranchStatus("trackHitsGlobal", true);
    inTree->SetBranchStatus("chi2Smoothed", true);

    // Go over the hits and indetify the best chi2
    // track passing through each of them
    std::unordered_set<int> badIdxs;
    badIdxs.reserve(eventEndIdx - eventStartIdx);
    std::unordered_map<TVector3, TrackHandle, TVector3Hash, TVector3Eq>
        clusterMap;
    clusterMap.reserve((eventEndIdx - eventStartIdx) * 5);
    for (int j = eventStartIdx; j < eventEndIdx; j++) {
      inTree->GetEntry(j);
      for (const auto &hit : *m_trackHitsGlobal) {
        auto &mapEntry = clusterMap[hit];
        if (mapEntry.chi2 > m_chi2Smoothed) {
          badIdxs.insert(mapEntry.treeIdx);
          mapEntry = {j, m_chi2Smoothed};
        } else {
          badIdxs.insert(j);
        }
      }
    }
    inTree->SetBranchStatus("*", true);

    // Get the unique track indices
    std::set<int> trackIdxs;
    for (const auto &[hit, track] : clusterMap) {
      const auto &[idx, chi2] = track;
      if (badIdxs.contains(idx)) {
        continue;
      }
      trackIdxs.insert(idx);
    }
    // TTree IO is faster with sequential readout
    for (auto j : trackIdxs) {
      inTree->GetEntry(j);
      outTree->Fill();
    }
    if (i % 10 == 0) {
      std::cout << i << "/" << nRanges << "\n";
    }
  }

  outFile->Write();
  outFile->Close();
}
