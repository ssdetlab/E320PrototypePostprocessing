#include "Analysis/SimPostprocessor.hpp"

int main() {
  SimPostprocessor postprocessor;
  SimPostprocessor::Options opt;

  opt.inPath =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/sim/"
      "test/global/misaligned";

  opt.inDataTreeName = "fitted-tracks";
  opt.outDataPath =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/sim/"
      "fitted-tracks-filtered.root";
  opt.outDataTreeName = "fitted-tracks";
  opt.events = 1e9;
  opt.skip = 0;
  opt.deduplictionAlgo = SimPostprocessor::DeduplictionAlgorithm::FullBestChi2;
  opt.ipGeoId = 8;
  opt.seed = 0;

  postprocessor.removeSharedClusters(opt);

  return 0;
}
