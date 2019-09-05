#pragma once

#include <vector>
#include <memory>

#include "graph.hpp"
#include "maxsat_interface.hpp"

namespace triangulator {

class BtGenCrMaxSat {
 public:
  BtGenCrMaxSat(std::vector<int> colors, std::vector<Lit> color_lits);
  void Construct(const Graph& graph, const std::vector<std::vector<int> >& pmcs, std::shared_ptr<MaxSatInterface> maxsat_interface);
 private:
  std::vector<int> colors_;
  std::vector<Lit> color_lits_;
};
} // namespace triangulator
