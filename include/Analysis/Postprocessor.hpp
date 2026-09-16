#pragma once

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "TLorentzVector.h"
#include "TMatrixD.h"
#include "TTree.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TVectorD.h"
#include "detail/FileHandle.hpp"

/// @brief class handling the E320 data postprocessing
class Postprocessor {
 public:
  /// @brief De-duplication algorithms enum
  enum DeduplictionAlgorithm : int {
    /// Select a random set of tracks
    /// that do not share any clusters
    Random = 0,
    /// Select the tracks that are the
    /// best chi2 tracks for every cluster
    /// contained in the track
    FullBestChi2 = 1
  };

  /// @brief postprocessing options struct
  struct Options {
    /// Data directory/file path
    std::string inPath;
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
    /// Deduplication algorithm
    DeduplictionAlgorithm deduplictionAlgo;
  };

  /// @brief Run shared clusters removal
  ///
  /// @param opt postprocessing options
  void removeSharedClusters(const Options& opt);

  /// @brief Check that no tracks in a sample share clusters
  ///
  /// @param opt postprocessing options
  void testClusterSharing(const Options& opt);

  /// @brief Run magnet subsampling
  ///
  /// @param opt postprocessing options
  void sampleMagnets(const Options& opt);

 private:
  /// @brief collect run data paths for a collection of runs
  ///
  /// @param opt postprocessing options
  ///
  /// @return map of run prefix to its path collection
  std::vector<std::string> collectDataPaths(const Options& opt) const;

  /// @brief get file handle for a given ROOT tree
  ///
  /// @param treeName name of the data tree
  /// @param paths run data paths
  ///
  /// @return file handle for run data reading
  FileHandle getInFileHandle(const std::string& treeName,
                             const std::vector<std::string>& paths);

 protected:
  /// EUDAQ trigger ID
  std::size_t m_eudaqTrgN = 0;

  /// EUDAQ DAQ run number
  std::size_t m_eudaqDAQNumber = 0;

  /// EUDAQ DAQ run start timestamp
  std::size_t m_eudaqRunStartTs = 0;

  /// EUDAQ DAQ run end timestamp
  std::size_t m_eudaqRunEndTs = 0;

  /// Event EPICS parity
  std::size_t m_epicsParity = 0;

  /// Event EPICS PID
  std::size_t m_epicsPulseId = 0;

  /// Event EPICS DAQ number
  std::size_t m_epicsDAQNumber = 0;

  /// Magnet configuration as seen by track
  double m_quad1Grad = 0;
  double m_quad2Grad = 0;
  double m_quad3Grad = 0;
  double m_xCorrectorStrength = 0;
  double m_dipoleStrength = 0;

  /// Measurement hits in the surface frame
  std::vector<TVector2>* m_trackHitsLocal = nullptr;

  /// Measurement hits in the global frame
  std::vector<TVector3>* m_trackHitsGlobal = nullptr;

  /// Direction measurements in the track frame
  std::vector<TVector3>* m_onSurfaceTrackDirection = nullptr;

  /// Covariances of the track hits
  std::vector<TMatrixD>* m_trackHitCovs = nullptr;

  /// Covariances of the track agnles
  std::vector<TMatrixD>* m_trackAngleCovs = nullptr;

  /// Geometry ids of the track hits
  std::vector<std::size_t>* m_geometryIds = nullptr;

  /// KF predicted track hits in the surface frame
  std::vector<TVector2>* m_predictedTrackHitsLocal = nullptr;
  std::vector<TVector2>* m_filteredTrackHitsLocal = nullptr;
  std::vector<TVector2>* m_smoothedTrackHitsLocal = nullptr;

  /// KF predicted track hits in the global frame
  std::vector<TVector3>* m_predictedTrackHitsGlobal = nullptr;
  std::vector<TVector3>* m_filteredTrackHitsGlobal = nullptr;
  std::vector<TVector3>* m_smoothedTrackHitsGlobal = nullptr;

  /// KF predicted on surface momenta in the track frame
  std::vector<TLorentzVector>* m_predictedOnSurfaceMomentum = nullptr;
  std::vector<TLorentzVector>* m_filteredOnSurfaceMomentum = nullptr;
  std::vector<TLorentzVector>* m_smoothedOnSurfaceMomentum = nullptr;

  /// KF residuals with respect to the measurements
  std::vector<TVector2>* m_predictedHitResiduals = nullptr;
  std::vector<TVector2>* m_filteredHitResiduals = nullptr;
  std::vector<TVector2>* m_smoothedHitResiduals = nullptr;

  std::vector<TVector2>* m_predictedAngleResiduals = nullptr;
  std::vector<TVector2>* m_filteredAngleResiduals = nullptr;
  std::vector<TVector2>* m_smoothedAngleResiduals = nullptr;

  /// KF pulls with respect to the measurements
  std::vector<TVector2>* m_predictedHitPulls = nullptr;
  std::vector<TVector2>* m_filteredHitPulls = nullptr;
  std::vector<TVector2>* m_smoothedHitPulls = nullptr;

  std::vector<TVector2>* m_predictedAnglePulls = nullptr;
  std::vector<TVector2>* m_filteredAnglePulls = nullptr;
  std::vector<TVector2>* m_smoothedAnglePulls = nullptr;

  /// Guessed bound track parameters
  TVectorD* m_boundTrackParametersGuess = nullptr;
  TMatrixD* m_boundTrackCovGuess = nullptr;

  /// KF predicted bound track parameters
  TVectorD* m_boundTrackParametersEst = nullptr;
  TMatrixD* m_boundTrackCovEst = nullptr;

  /// Initial guess of the momentum at the IP
  TLorentzVector* m_originMomentumGuess = nullptr;

  /// Initial guess of the vertex at the IP
  TVector3* m_vertexGuess = nullptr;

  /// KF predicted momentum at the IP
  TLorentzVector* m_originMomentumEst = nullptr;

  /// KF predicted vertex at the IP
  TVector3* m_vertexEst = nullptr;

  /// Chi2 of the track
  /// with respect ot the
  /// measurement
  double m_chi2Predicted = 0;
  double m_chi2Filtered = 0;
  double m_chi2Smoothed = 0;

  /// Number of degrees of freedom
  /// of the track
  std::size_t m_ndf = 0;

  /// Track ID
  std::size_t m_trackId = 0;

  /// Event ID
  std::size_t m_eventId = 0;

  /// PDG ID
  int m_pdgId = 0;

  /// Charge
  int m_charge = 0;

  // HT cell intersections count
  std::size_t m_xCount = 0;
};
