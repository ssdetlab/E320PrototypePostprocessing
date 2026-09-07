#include "Analysis/Postprocessor.hpp"

#include <cmath>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "detail/TVector3Hash.hpp"

std::vector<std::string> Postprocessor::collectDataPaths(
    const Options &opt) const {
  std::cout << "Collecting data paths from\n" << opt.inPath << "\n";

  // Collect paths
  std::vector<std::string> paths;
  std::size_t idx = 0;
  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(opt.inPath)) {
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
  TTree *dataTree = nullptr;
  TTree *selectionTree = nullptr;
  TChain *chainOwner = new TChain(treeName.c_str(), treeName.c_str());
  if (paths.size() == 1) {
    file = new TFile(paths.at(0).c_str());
    dataTree = file->Get<TTree>(treeName.c_str());
    selectionTree = file->Get<TTree>(treeName.c_str());
  } else {
    chainOwner = new TChain(treeName.c_str());
    for (const auto &path : paths) {
      chainOwner->Add(path.c_str());
    }
    dataTree = dynamic_cast<TTree *>(chainOwner);
    selectionTree = dynamic_cast<TTree *>(chainOwner);
  }

  // Set event ID branch
  dataTree->SetBranchAddress("eventId", &m_eventId);
  if (dataTree->GetBranch("eventId") == nullptr) {
    throw std::invalid_argument("Missing eventId branch");
  }
  auto nEntries = static_cast<std::size_t>(dataTree->GetEntries());

  // Get event ranges
  std::cout << "Getting event ranges\n";
  std::vector<std::pair<std::uint32_t, std::uint32_t>> eventRanges;
  std::unordered_set<std::size_t> eventIds;
  dataTree->GetEntry(0);
  std::size_t currentEventId = m_eventId;
  std::size_t currentIdx = 0;
  for (std::size_t i = 0; i < nEntries - 1; i++) {
    dataTree->GetEntry(i);
    eventIds.insert(m_eventId);
    if (m_eventId != currentEventId) {
      eventRanges.push_back({currentIdx, i});
      currentIdx = i;
      currentEventId = m_eventId;
    }
    if (i % 100000 == 0) {
      std::cout << i << "/" << nEntries << "\n";
    }
  }
  eventRanges.push_back({currentIdx, nEntries});
  std::cout << "Entries " << nEntries << "\n";
  std::cout << "Unique event IDs " << eventIds.size() << "\n";
  std::cout << "Total ranges found " << eventRanges.size() << "\n";
  std::cout << "\n\n\n\n";
  // throw std::runtime_error("ERR");

  // Event meta data
  dataTree->SetBranchAddress("eudaqTrgN", &m_eudaqTrgN);
  dataTree->SetBranchAddress("eudaqDAQNumber", &m_eudaqDAQNumber);
  dataTree->SetBranchAddress("eudaqRunStartTs", &m_eudaqRunStartTs);
  dataTree->SetBranchAddress("eudaqRunEndTs", &m_eudaqRunEndTs);
  dataTree->SetBranchAddress("epicsParity", &m_epicsParity);
  dataTree->SetBranchAddress("epicsPulseId", &m_epicsPulseId);
  dataTree->SetBranchAddress("epicsDAQNumber", &m_epicsDAQNumber);

  // Magnet configuration as seen by track
  dataTree->SetBranchAddress("quad1Grad", &m_quad1Grad);
  dataTree->SetBranchAddress("quad2Grad", &m_quad2Grad);
  dataTree->SetBranchAddress("quad3Grad", &m_quad3Grad);
  dataTree->SetBranchAddress("xCorrectorStrength", &m_xCorrectorStrength);
  dataTree->SetBranchAddress("dipoleStrength", &m_dipoleStrength);

  // Measurement hits in the surface frame
  dataTree->SetBranchAddress("trackHitsLocal", &m_trackHitsLocal);

  // Measurement hits in the global frame
  dataTree->SetBranchAddress("trackHitsGlobal", &m_trackHitsGlobal);

  // Direction measurements in the track frame
  dataTree->SetBranchAddress("onSurfaceTrackDirection",
                             &m_onSurfaceTrackDirection);

  // Covariances of the track hits
  dataTree->SetBranchAddress("trackHitCovs", &m_trackHitCovs);

  // Covariances of the track agnles
  dataTree->SetBranchAddress("trackAngleCovs", &m_trackAngleCovs);

  // Geometry ids of the track hits
  dataTree->SetBranchAddress("geometryIds", &m_geometryIds);

  // KF predicted track hits in the surface frame
  dataTree->SetBranchAddress("predictedTrackHitsLocal",
                             &m_predictedTrackHitsLocal);
  dataTree->SetBranchAddress("filteredTrackHitsLocal",
                             &m_filteredTrackHitsLocal);
  dataTree->SetBranchAddress("smoothedTrackHitsLocal",
                             &m_smoothedTrackHitsLocal);

  // KF predicted track hits in the global frame
  dataTree->SetBranchAddress("predictedTrackHitsGlobal",
                             &m_predictedTrackHitsGlobal);
  dataTree->SetBranchAddress("filteredTrackHitsGlobal",
                             &m_filteredTrackHitsGlobal);
  dataTree->SetBranchAddress("smoothedTrackHitsGlobal",
                             &m_smoothedTrackHitsGlobal);

  // KF predicted on surface momenta in the track frame
  dataTree->SetBranchAddress("predictedOnSurfaceMomentum",
                             &m_predictedOnSurfaceMomentum);
  dataTree->SetBranchAddress("filteredOnSurfaceMomentum",
                             &m_filteredOnSurfaceMomentum);
  dataTree->SetBranchAddress("smoothedOnSurfaceMomentum",
                             &m_smoothedOnSurfaceMomentum);

  // KF residuals with respect to the measurements
  dataTree->SetBranchAddress("predictedHitResiduals", &m_predictedHitResiduals);
  dataTree->SetBranchAddress("filteredHitResiduals", &m_filteredHitResiduals);
  dataTree->SetBranchAddress("smoothedHitResiduals", &m_smoothedHitResiduals);

  dataTree->SetBranchAddress("predictedAngleResiduals",
                             &m_predictedAngleResiduals);
  dataTree->SetBranchAddress("filteredAngleResiduals",
                             &m_filteredAngleResiduals);
  dataTree->SetBranchAddress("smoothedAngleResiduals",
                             &m_smoothedAngleResiduals);

  // KF pulls with respect to the measurements
  dataTree->SetBranchAddress("predictedHitPulls", &m_predictedHitPulls);
  dataTree->SetBranchAddress("filteredHitPulls", &m_filteredHitPulls);
  dataTree->SetBranchAddress("smoothedHitPulls", &m_smoothedHitPulls);

  dataTree->SetBranchAddress("predictedAnglePulls", &m_predictedAnglePulls);
  dataTree->SetBranchAddress("filteredAnglePulls", &m_filteredAnglePulls);
  dataTree->SetBranchAddress("smoothedAnglePulls", &m_smoothedAnglePulls);

  // Guessed bound track parameters
  dataTree->SetBranchAddress("boundTrackParametersGuess",
                             &m_boundTrackParametersGuess);
  dataTree->SetBranchAddress("boundTrackCovGuess", &m_boundTrackCovGuess);

  // KF predicted bound track parameters
  dataTree->SetBranchAddress("boundTrackParametersEst",
                             &m_boundTrackParametersEst);
  dataTree->SetBranchAddress("boundTrackCovEst", &m_boundTrackCovEst);

  // Initial guess of the momentum at the IP
  dataTree->SetBranchAddress("originMomentumGuess", &m_originMomentumGuess);

  // Initial guess of the vertex at the IP
  dataTree->SetBranchAddress("vertexGuess", &m_vertexGuess);

  // KF predicted momentum at the IP
  dataTree->SetBranchAddress("originMomentumEst", &m_originMomentumEst);

  // KF predicted vertex at the IP
  dataTree->SetBranchAddress("vertexEst", &m_vertexEst);

  // Chi2 of the track with respect ot the measurement
  dataTree->SetBranchAddress("chi2Predicted", &m_chi2Predicted);
  dataTree->SetBranchAddress("chi2Filtered", &m_chi2Filtered);
  dataTree->SetBranchAddress("chi2Smoothed", &m_chi2Smoothed);

  // Number of degrees of freedom of the track
  dataTree->SetBranchAddress("ndf", &m_ndf);

  // Track ID
  dataTree->SetBranchAddress("trackId", &m_trackId);

  // PDG ID
  dataTree->SetBranchAddress("pdgId", &m_pdgId);

  // Charge
  dataTree->SetBranchAddress("charge", &m_charge);

  dataTree->SetBranchAddress("xCount", &m_xCount);

  // Set selection tree branches
  selectionTree->SetBranchAddress("xCorrectorStrength", &m_xCorrectorStrength);
  selectionTree->SetBranchAddress("dipoleStrength", &m_dipoleStrength);
  selectionTree->SetBranchAddress("trackHitsGlobal", &m_trackHitsGlobal);
  selectionTree->SetBranchAddress("chi2Smoothed", &m_chi2Smoothed);

  return {file, chainOwner, dataTree, selectionTree, eventRanges};
}

void Postprocessor::removeSharedClusters(const Options &opt) {
  std::vector<std::string> paths = opt.inPath.ends_with(".root")
                                       ? std::vector({opt.inPath})
                                       : collectDataPaths(opt);

  // Get data tree
  auto [inFile, inChain, dataTree, selectionTree, eventRanges] =
      getInFileHandle(opt.inDataTreeName, paths);
  std::size_t nRanges = eventRanges.size();

  // Initilize output file
  TFile *outFile = new TFile(opt.outDataPath.c_str(), "RECREATE");
  TTree *outTree = nullptr;
  outTree = dataTree->CloneTree(0);
  dataTree->CopyAddresses(outTree);

  // Remove cluster sharing
  std::set<int> uniqueTrackIdxs;

  std::size_t startIdx = opt.skip;
  std::size_t endIdx = std::min(opt.skip + opt.events, nRanges);
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
      selectionTree->GetEntry(j);
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

  // Fill the tree
  for (int j : uniqueTrackIdxs) {
    dataTree->GetEntry(j);
    outTree->Fill();
  }

  outFile->Write();
  outFile->Close();
}

void Postprocessor::testClusterSharing(const Options &opt) {
  std::vector<std::string> paths = opt.inPath.ends_with(".root")
                                       ? std::vector({opt.inPath})
                                       : collectDataPaths(opt);

  // Get data tree
  auto [inFile, inChain, dataTree, selectionTree, eventRanges] =
      getInFileHandle(opt.inDataTreeName, paths);
  std::size_t nRanges = eventRanges.size();

  std::size_t startIdx = opt.skip;
  std::size_t endIdx = std::min(opt.skip + opt.events, nRanges);
  for (std::size_t i = startIdx; i < endIdx; i++) {
    auto [eventStartIdx, eventEndIdx] = eventRanges.at(i);

    for (int j = eventStartIdx; j < eventEndIdx - 1; j++) {
      selectionTree->GetEntry(j);
      auto hitsJ = *m_trackHitsGlobal;
      for (int k = j + 1; k < eventEndIdx; k++) {
        selectionTree->GetEntry(k);
        auto hitsK = *m_trackHitsGlobal;
        for (const auto &hitJ : hitsJ) {
          for (const auto &hitK : hitsK) {
            // std::cout << "J: " << j << ", K: " << k << ", hitJ = [" << hitJ.X()
            //           << " " << hitJ.Y() << " " << hitJ.Z() << "], hitK = ["
            //           << hitJ.X() << " " << hitJ.Y() << " " << hitJ.Z()
            //           << "]\n";
            if (hitJ == hitK) {
              throw std::runtime_error("ERR");
            }
          }
        }
      }
    }
    if (i % 1000 == 0) {
      std::cout << i << "/" << nRanges << "\n";
    }
  }
}

void Postprocessor::sampleMagnets(const Options &opt) {
  std::vector<std::string> paths = opt.inPath.ends_with(".root")
                                       ? std::vector({opt.inPath})
                                       : collectDataPaths(opt);

  // Get data tree
  auto [inFile, inChain, dataTree, selectionTree, eventRanges] =
      getInFileHandle(opt.inDataTreeName, paths);
  std::size_t nRanges = eventRanges.size();

  // Initilize output file
  TFile *outFile = new TFile(opt.outDataPath.c_str(), "RECREATE");
  TTree *outTree = nullptr;
  outTree = dataTree->CloneTree(0);
  dataTree->CopyAddresses(outTree);

  // Set up magnet ranges
  std::vector<std::pair<double, double>> dipoleRanges{
      {-0.23, -0.21}, {-0.21, -0.19}, {-0.18, -0.17}, {-0.16, -0.15}};

  std::vector<std::pair<double, double>> xcorRanges{
      {-0.054, -0.050}, {-0.018, -0.014}, {0.024, 0.027}, {0.027, 0.030}};

  // Count mag filed configurations
  std::unordered_map<std::size_t, std::set<int>> magnetMap;

  std::size_t startIdx = opt.skip;
  std::size_t endIdx = std::min(opt.skip + opt.events, nRanges);
  for (std::size_t i = startIdx; i < endIdx; i++) {
    auto [eventStartIdx, eventEndIdx] = eventRanges.at(i);

    for (int j = eventStartIdx; j < eventEndIdx; j++) {
      selectionTree->GetEntry(j);

      std::size_t magId = 0;
      for (std::size_t j = 0; j < dipoleRanges.size(); j++) {
        const auto &[low, high] = dipoleRanges.at(j);
        if (m_dipoleStrength >= low && m_dipoleStrength <= high) {
          magId = 10 * j;
        }
      }
      for (std::size_t j = 0; j < xcorRanges.size(); j++) {
        const auto &[low, high] = xcorRanges.at(j);
        if (m_xCorrectorStrength >= low && m_xCorrectorStrength <= high) {
          magId += j;
        }
      }

      magnetMap[magId].insert(j);
    }

    if (i % 1000 == 0) {
      std::cout << i << "/" << nRanges << "\n";
    }
  }

  std::size_t minSize = std::numeric_limits<std::size_t>::max();
  for (const auto &[magId, idxs] : magnetMap) {
    std::size_t currentSize = idxs.size();
    if (currentSize < minSize) {
      minSize = currentSize;
    }
  }

  // Fill the tree
  for (const auto &[magId, idxs] : magnetMap) {
    std::cout << magId << ": " << idxs.size() << "\n";
    std::size_t count = 0;
    for (int j : idxs) {
      dataTree->GetEntry(j);
      outTree->Fill();

      count++;
      if (count == minSize) {
        break;
      }
    }
    std::cout << count << "\n";
  }

  outFile->Write();
  outFile->Close();
}
