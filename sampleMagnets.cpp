#include "Analysis/Postprocessor.hpp"

int main() {
  Postprocessor postprocessor;

  Postprocessor::Options opt;
  opt.inPath =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/data/"
      "alignment/local_2026_data/it1/sig/filtered/fitted-tracks-filtered.root";

  opt.inDataTreeName = "fitted-tracks";
  opt.outDataPath =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/data/"
      "fitted-tracks-filtered.root";
  opt.outDataTreeName = "fitted-tracks";
  opt.events = 1e9;
  opt.skip = 0;

  postprocessor.sampleMagnets(opt);

  return 0;
}

