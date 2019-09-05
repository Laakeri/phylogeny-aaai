 #pragma once

#include <vector>

#include "graph.hpp"
#include "phyl_mat.hpp"

namespace triangulator {
class PhylogenyPreprocessor {
 public:
  // Returns pairs of (graph, colors)
  std::vector<std::pair<Graph, std::vector<int>>> Preprocess(const PhylMat& pm);
 private:
  std::vector<std::pair<Graph, std::vector<int>>> instances_;
  void Preprocess1(const Graph& graph, const PhylMat& opm);
  void AddInstance(const Graph& graph, const PhylMat& opm);
};
} // namespace triangulator