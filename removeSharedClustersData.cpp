#include "Analysis/Postprocessor.hpp"

int main() {
  Postprocessor postprocessor;
  Postprocessor::Options opt;

  opt.inPath =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/data/"
      "alignment/local_2026_data/"
      "test/misaligned/fitted-tracks.root";

  opt.inDataTreeName = "fitted-tracks";
  opt.outDataPath =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/data/"
      "fitted-tracks-filtered.root";
  opt.events = 1e9;
  opt.skip = 0;
  opt.deduplictionAlgo = Postprocessor::DeduplictionAlgorithm::FullBestChi2;
  opt.ipGeoId = 8;
  opt.seed = 0;

  postprocessor.removeSharedClusters(opt);

  return 0;
}
