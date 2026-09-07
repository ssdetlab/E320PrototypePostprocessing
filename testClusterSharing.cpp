#include "Analysis/Postprocessor.hpp"
// #include "Analysis/SimPostprocessor.hpp"

int main() {
  Postprocessor postprocessor;
  Postprocessor::Options opt;

  // SimPostprocessor postprocessor;
  // SimPostprocessor::Options opt;

  opt.inPath =
      // "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/data/"
      // "alignment/local_2026_data/cluster_error_test/sig/test/raw/fitted-tracks.root";
      // "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/sim/"
      // "alignment/local/sig_bkg/misaligned/fitted-tracks.root";
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/data/"
      "alignment/local_2026_data/cluster_error_test/sig/filtered/"
      "fitted-tracks-filtered.root";

  opt.inDataTreeName = "fitted-tracks";
  opt.outDataPath =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/sim/"
      "fitted-tracks-filtered.root";
  opt.outDataTreeName = "fitted-tracks";
  opt.events = 1e9;
  opt.skip = 0;

  postprocessor.testClusterSharing(opt);

  return 0;
}
