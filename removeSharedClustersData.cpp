#include "Analysis/Postprocessor.hpp"

int main() {
  Postprocessor postprocessor;
  Postprocessor::Options opt;

  opt.inPath =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/data/"
      "alignment/local_2026_data/test";
  // "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/sim/"
  // "alignment/cluster_error_test/allpix2_errors/misaligned/fitted-tracks.root";

  opt.inDataTreeName = "fitted-tracks";
  opt.outDataPath =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/sim/"
      "fitted-tracks-filtered.root";
  opt.outDataTreeName = "fitted-tracks";
  opt.events = 1e9;
  opt.skip = 0;
  opt.deduplictionAlgo = Postprocessor::DeduplictionAlgorithm::Random;

  postprocessor.removeSharedClusters(opt);

  return 0;
}
