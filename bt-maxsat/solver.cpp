#include "solver.hpp"

#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <cassert>

#include "graph.hpp"
#include "utils.hpp"
#include "comb_enum.hpp"
#include "phyl_mat.hpp"
#include "phylogenypreprocessor.hpp"
#include "bt_gencr_maxsat.hpp"
#include "maxsat_interface.hpp"
#include "maxhs_interface.hpp"

namespace triangulator {

int GeneralCharRemoveMaxSat(const PhylMat& pm) {
  Timer pmc_timer;
  pmc_timer.start();
  Log::Write(3, "Graph size ", pm.GetGraph().n());
  PhylogenyPreprocessor ppp;
  auto instances = ppp.Preprocess(pm);

  std::shared_ptr<MaxSatInterface> maxsat_interface = std::make_shared<MaxhsInterface>();
  std::vector<Lit> color_vars;
  for (int i = 0; i < pm.m(); i++) {
    color_vars.push_back(maxsat_interface->NewVar());
    maxsat_interface->AddSoftClause({-color_vars.back()}, 1);
  }

  for (const std::pair<Graph, std::vector<int>>& instance : instances) {
    Log::Write(3, "Inst ", instance.first.n(), " ", instance.first.m());
    auto pmcs = comb_enumerator::Pmcs(instance.first);
    Log::Write(3, "Pmcs ", pmcs.size());

    BtGenCrMaxSat bt(instance.second, color_vars);
    bt.Construct(instance.first, pmcs, maxsat_interface);
  }
  pmc_timer.stop();
  Log::Write(3, "Timer pmcs ", pmc_timer.getTime().count());
  Timer maxsat_timer;
  maxsat_timer.start();
  assert(maxsat_interface->Solve());
  maxsat_timer.stop();
  Log::Write(3, "Timer maxsat ", maxsat_timer.getTime().count());
  std::vector<int> sol;
  for (int i = 0; i < pm.m(); i++) {
    if (maxsat_interface->SolutionValue(color_vars[i])) {
      sol.push_back(i);
    }
  }
  Log::Write(3, "remc ", sol.size());
  std::stringstream ss;
  for (int x : sol) {
    ss << x << " ";
  }
  Log::Write(3, ss.str());
  return sol.size();
}

PhylMat PhylPreprocess(const PhylMat& pm) {
  Log::Write(3, "Graph size ", pm.GetGraph().n());
  PhylogenyPreprocessor ppp;
  auto instances = ppp.Preprocess(pm);
  std::set<int> rel_cols;
  for (const std::pair<Graph, std::vector<int>>& instance : instances) {
    for (int i = 0; i < instance.first.n(); i++) {
      rel_cols.insert(instance.second[i]);
    }
  }
  if (rel_cols.size() == 0) {
    rel_cols.insert(0);
  }
  Matrix<int> new_mat(pm.n(), rel_cols.size());
  for (int i = 0; i < pm.n(); i++) {
    int idx = 0;
    for (int ii = 0; ii < pm.m(); ii++) {
      if (rel_cols.count(ii)) {
        new_mat[i][idx] = pm.GetMat(i, ii);
        idx++;
      }
    }
    assert(idx == (int)rel_cols.size());
  }
  for (int i = 0; i < new_mat.Columns(); i++) {
    std::map<int, int> cm;
    for (int ii = 0; ii < new_mat.Rows(); ii++) {
      if (new_mat[ii][i] != -1) {
        if (!cm.count(new_mat[ii][i])) {
          int t = cm.size();
          cm[new_mat[ii][i]] = t;
        }
        new_mat[ii][i] = cm[new_mat[ii][i]];
      }
    }
  }
  return PhylMat(new_mat);
}
} // namespace triangulator
