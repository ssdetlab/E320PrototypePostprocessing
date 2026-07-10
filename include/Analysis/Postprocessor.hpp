#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TMatrixD.h"
#include "TTree.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TVectorD.h"

/// @brief class handling the E320 data postprocessing
class Postprocessor {
 public:
  /// Shorthand for the ROOT file handles
  using FileHandle = std::tuple<TFile *, TChain *, TTree *>;

  struct TrackHandle {
    int treeIdx = -1;
    double chi2 = std::numeric_limits<double>::max();
  };

  /// @brief preprocessing options struct
  struct Options {
    /// Data directory path
    std::string inDataDir;
    /// Data tree name
    std::string inDataTreeName;
    /// Output directory path
    std::string outDataPath;
    /// Output data tree name
    std::string outDataTreeName;
    /// Number of events to process
    std::size_t events;
    /// Number of events to skip in the beginning
    std::size_t skip;
  };

  /// @brief run data paths container
  struct PathCollection {
    /// Prefix of the run files
    std::string runPrefix = "";
    /// Parent path of the run files
    std::string parentPath = "";
    /// Collection of the run files
    std::vector<std::string> inPaths{};
    /// Output path
    std::string outPath = "";
    /// Number of events to skip in the beginning
    std::size_t skip = 0;
    /// Number of events to skip in the end
    std::size_t stop = 0;
  };

  /// @brief Run data preprocessing
  ///
  /// @param opt preprocessing options
  void processFiles(const Options &opt);

 private:
  /// @brief collect run data paths for a collection of runs
  ///
  /// @param opt preprocessing options
  ///
  /// @return map of run prefix to its path collection
  std::vector<std::string> collectDataPaths(const Options &opt) const;

  /// @brief get file handle for a given ROOT tree
  ///
  /// @param treeName name of the data tree
  /// @param paths run data paths
  ///
  /// @return file handle for run data reading
  FileHandle getInFileHandle(const std::string &treeName,
                             const std::vector<std::string> &paths);

  std::vector<std::pair<std::uint32_t, std::uint32_t>> getEventRanges(
      TTree *tree) const;

 protected:
  /// EPICS meta
  std::size_t m_epicsParity = 0;
  std::size_t m_epicsPID = 0;
  std::size_t m_epicsDAQNumber = 0;

  /// Measurement hits
  std::vector<TVector3> *m_trackHitsGlobal = nullptr;
  std::vector<TVector2> *m_trackHitsLocal = nullptr;

  /// Covariances of the track hits
  std::vector<TMatrixD> *m_trackHitsCovs = nullptr;

  /// Geometry ids of the track hits
  std::vector<std::size_t> *m_geometryIds = nullptr;

  /// KF predicted track hits
  std::vector<TVector3> *m_predictedTrackHitsGlobal = nullptr;
  std::vector<TVector3> *m_filteredTrackHitsGlobal = nullptr;
  std::vector<TVector3> *m_smoothedTrackHitsGlobal = nullptr;

  std::vector<TVector2> *m_predictedTrackHitsLocal = nullptr;
  std::vector<TVector2> *m_filteredTrackHitsLocal = nullptr;
  std::vector<TVector2> *m_smoothedTrackHitsLocal = nullptr;

  /// KF residuals with respect to the measurements
  std::vector<TVector2> *m_predictedResiduals = nullptr;
  std::vector<TVector2> *m_filteredResiduals = nullptr;
  std::vector<TVector2> *m_smoothedResiduals = nullptr;

  /// KF pulls with respect to the measurements
  std::vector<TVector2> *m_predictedPulls = nullptr;
  std::vector<TVector2> *m_filteredPulls = nullptr;
  std::vector<TVector2> *m_smoothedPulls = nullptr;

  /// Chi2 of the track
  /// with respect ot the
  /// measurement
  double m_chi2Predicted = 0;
  double m_chi2Filtered = 0;
  double m_chi2Smoothed = 0;

  /// Number of degrees of freedom
  /// of the track
  std::size_t m_ndf = 0;

  /// TrackId
  std::size_t m_trackId = 0;

  /// EventId
  std::size_t m_eventId = 0;

  /// PDG ID
  int m_pdgId = 0;

  /// Charge
  int m_charge = 0;

  /// Guessed bound track parameters
  TVectorD *m_boundTrackParametersGuess = nullptr;
  TMatrixD *m_boundTrackCovGuess = nullptr;

  /// KF predicted bound track parameters
  TVectorD *m_boundTrackParametersEst = nullptr;
  TMatrixD *m_boundTrackCovEst = nullptr;

  /// Initial guess of the momentum at the IP
  TLorentzVector *m_originMomentumGuess = nullptr;

  /// Initial guess of the vertex at the IP
  TVector3 *m_vertexGuess = nullptr;

  /// KF predicted momentum at the IP
  TLorentzVector *m_originMomentumEst = nullptr;

  /// KF predicted vertex at the IP
  TVector3 *m_vertexEst = nullptr;
};
