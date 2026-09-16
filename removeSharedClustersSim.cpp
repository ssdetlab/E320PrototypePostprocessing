#include "Analysis/SimPostprocessor.hpp"

int main() {
  SimPostprocessor postprocessor;
  SimPostprocessor::Options opt;

  opt.inPath =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/sim/"
      "alignment/local/sig/misaligned";

  opt.inDataTreeName = "fitted-tracks";
  opt.outDataPath =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/sim/"
      "fitted-tracks-filtered.root";
  opt.outDataTreeName = "fitted-tracks";
  opt.events = 1e9;
  opt.skip = 0;
  opt.deduplictionAlgo = SimPostprocessor::DeduplictionAlgorithm::Random;

  postprocessor.removeSharedClusters(opt);

  return 0;
}

