#pragma once

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"

/// @brief File handle struct
struct FileHandle {
  /// File pointer
  TFile* file;
  /// Chain owner pointer
  TChain* chainOwner;
  /// Tree storing the whole track data
  TTree* dataTree;
  /// Tree storing selection branches
  TTree* selectionTree;
  /// Ranges of the events
  std::vector<std::pair<std::uint32_t, std::uint32_t>> eventRanges;
};
