#include "phylogenypreprocessor.hpp"

#include <vector>
#include <cassert>

#include "graph.hpp"
#include "mcs.hpp"

namespace triangulator {

std::vector<std::pair<Graph, std::vector<int>>> PhylogenyPreprocessor::Preprocess(const PhylMat& pm) {
  instances_.clear();
  Graph graph = pm.GetGraph();
  Preprocess1(graph, pm);
  return instances_;
}

void PhylogenyPreprocessor::Preprocess1(const Graph& graph, const PhylMat& opm) {
  if (graph.n() == 0) return;
  mcs::McsMOutput minimal_triangulation = mcs::McsM(graph);
  if (minimal_triangulation.fill_edges.size() == 0) {
    // Does not contribute to CR.
    return;
  }
  bool bad_fill = false;
  for (auto e : minimal_triangulation.fill_edges) {
    if (opm.Color(graph.MapBack(e.first)) == opm.Color(graph.MapBack(e.second))) {
      bad_fill = true;
      break;
    }
  }
  if (!bad_fill) {
    // Found perfect fill for this component.
    Log::Write(3, "Eliminated component");
    return;
  }
  std::vector<Graph> atoms = mcs::Atoms(graph, minimal_triangulation);
  if (atoms.size() == 1) {
    AddInstance(graph, opm);
  } else {
    for (Graph& atom : atoms) {
      atom.InheritMap(graph);
      Preprocess1(atom, opm);
    }
  }
}

void PhylogenyPreprocessor::AddInstance(const Graph& graph, const PhylMat& opm) {
  std::vector<int> color(graph.n());
  for (int i = 0; i < graph.n(); i++) {
    int og = graph.MapBack(i);
    assert(og >= 0 && og < opm.GetGraph().n());
    color[i] = opm.Color(og);
  }
  instances_.push_back({graph, color});
}
} // namespace triangulator