#pragma once

#include <limits>

/// @brief Track handle struct
struct TrackHandle {
  int treeIdx = -1;
  double chi2 = std::numeric_limits<double>::max();
  std::size_t magId = 0;
};
