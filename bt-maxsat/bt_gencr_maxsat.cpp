#include "bt_gencr_maxsat.hpp"

#include <vector>
#include <map>
#include <cassert>
#include <queue>
#include <iostream>
#include <set>

#include "graph.hpp"
#include "id_set.hpp"
#include "maxsat_interface.hpp"

namespace triangulator {
BtGenCrMaxSat::BtGenCrMaxSat(std::vector<int> colors, std::vector<Lit> color_lits) : colors_(colors), color_lits_(color_lits) { }
void BtGenCrMaxSat::Construct(const Graph& graph, const std::vector<std::vector<int> >& pmcs, std::shared_ptr<MaxSatInterface> maxsat_interface) {
  std::vector<std::tuple<int, int, int> > triplets;
  IdSet<std::vector<int> > separators, components;
  std::set<std::pair<int, int> > blocks;
  std::vector<int> all_vertices(graph.n());
  for (int i = 0; i < graph.n(); i++) all_vertices[i] = i;
  int none_sep_id = separators.Insert({});
  int all_comp_id = components.Insert(all_vertices);
  for (int i = 0; i < pmcs.size(); i++) {
    const auto& pmc = pmcs[i];
    for (const auto& out_component : graph.Components(pmc)) {
      std::vector<int> separator = graph.Neighbors(out_component);
      assert(separator.size() < pmc.size());
      std::vector<char> in_separator(graph.n());
      for (int v : separator) in_separator[v] = true;
      std::vector<int> component;
      int found = 0;
      for (int v : pmc) {
        if (!in_separator[v]) {
          component = graph.FindComponentAndMark(v, in_separator);
          found++;
        }
      }
      assert(found == 1);
      std::sort(separator.begin(), separator.end());
      std::sort(component.begin(), component.end());
      int sep_id = separators.Insert(separator);
      int comp_id = components.Insert(component);
      blocks.insert({sep_id, comp_id});
      triplets.push_back(std::make_tuple(i, sep_id, comp_id));
    }
    blocks.insert({none_sep_id, all_comp_id});
    triplets.push_back(std::make_tuple(i, none_sep_id, all_comp_id));
  }
  for (auto block : blocks) {
    triplets.push_back(std::make_tuple(-1, block.first, block.second));
  }
  auto cmp = [&](std::tuple<int, int, int> a, std::tuple<int, int, int> b) {
    int size_a = separators.Get(std::get<1>(a)).size() + components.Get(std::get<2>(a)).size();
    int size_b = separators.Get(std::get<1>(b)).size() + components.Get(std::get<2>(b)).size();
    if (size_a == size_b) {
      // triplets with -1 in <0> last.
      return std::get<0>(a) > std::get<0>(b);
    }
    return size_a < size_b;
  };
  std::sort(triplets.begin(), triplets.end(), cmp);

  std::vector<Lit> pmc_vars;
  for (int i = 0; i < (int)pmcs.size(); i++) {
    std::set<int> bad_colors;
    for (int j = 0; j < (int)pmcs[i].size(); j++) {
      for (int jj = j+1; jj < (int)pmcs[i].size(); jj++) {
        if (colors_[pmcs[i][j]] == colors_[pmcs[i][jj]]) {
          bad_colors.insert(colors_[pmcs[i][j]]);
        }
      }
    }
    if (bad_colors.size() == 0) {
      pmc_vars.push_back(Lit::TrueLit());
    } else {
      pmc_vars.push_back(maxsat_interface->NewVar());
      for (int c : bad_colors) {
        maxsat_interface->AddClause({-pmc_vars.back(), color_lits_[c]});
      }
    }
  }
  std::map<std::pair<int, int>, Lit> block_vars;
  std::map<std::pair<int, int>, std::vector<Lit>> child_trips;

  for (const auto& triplet : triplets) {
    std::pair<int, int> this_state = {std::get<1>(triplet), std::get<2>(triplet)};
    if (std::get<0>(triplet) == -1) {
      assert(block_vars.count(this_state) == 0);

      block_vars[this_state] = maxsat_interface->NewVar();
      std::vector<Lit> clause = child_trips[this_state];
      clause.push_back(-block_vars[this_state]);
      maxsat_interface->AddClause(clause);
    }
    else {
      const auto& pmc = pmcs[std::get<0>(triplet)];
      const auto& component = components.Get(this_state.second);

      Lit trip_var = maxsat_interface->NewVar();
      maxsat_interface->AddClause({-trip_var, pmc_vars[std::get<0>(triplet)]});

      std::vector<char> in_pmc(graph.n());
      for (int v : pmc) in_pmc[v] = true;
      for (int v : component) {
        if (!in_pmc[v]) {
          auto child_component = graph.FindComponentAndMark(v, in_pmc);
          auto child_separator = graph.Neighbors(child_component);
          std::sort(child_component.begin(), child_component.end());
          std::sort(child_separator.begin(), child_separator.end());
          std::pair<int, int> child_state = {separators.IdOf(child_separator), components.IdOf(child_component)};

          assert(block_vars.count(child_state));
          maxsat_interface->AddClause({-trip_var, block_vars[child_state]});
        }
      }
      child_trips[this_state].push_back(trip_var);
    }
  }
  assert(block_vars.count({none_sep_id, all_comp_id}));
  maxsat_interface->AddClause({block_vars[{none_sep_id, all_comp_id}]});
}
} // namespace triangulator