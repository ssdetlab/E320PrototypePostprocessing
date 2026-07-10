#include "Analysis/Postprocessor.hpp"

int main() {
  Postprocessor postprocessor;

  Postprocessor::Options opt;
  opt.inDataDir =
      "/home/romanurmanov/work/E320/E320Prototype/E320Prototype_analysis/data/"
      "data_analysis/june_2026/06_23_2026_3-5GeV_focus_sig/combined";
  opt.inDataTreeName = "fitted-tracks";
  opt.outDataPath = "fitted-tracks-filtered.root";
  opt.outDataTreeName = "fitted-tracks";

  postprocessor.processFiles(opt);

  return 0;
}
