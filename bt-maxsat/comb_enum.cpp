#include "comb_enum.hpp"

#include <vector>
#include <cassert>
#include <set>

#include "graph.hpp"
#include "mcs.hpp"
#include "utils.hpp"

namespace triangulator {
namespace comb_enumerator {
namespace {

void dfs2(const Graph& graph, int x, std::vector<int>& u) {
  assert(u[x] == 0);
  u[x] = 1;
  for (int nx : graph.Neighbors(x)) {
    if (u[nx] == 0) dfs2(graph, nx, u);
    else if (u[nx] == 2) {
      u[nx] = 3;
    }
  }
}

bool extend2(const Graph& graph, const std::vector<int>& pmc, int x) {
  std::vector<int> u(graph.n());
  for (int v : pmc) u[v] = 2;
  dfs2(graph, x, u);
  for (int v : pmc) {
    if (u[v] != 3) return false;
  }
  return true;
}

bool IsPmc(const Graph& graph, const std::vector<int>& pmc) {
  if (graph.HasNFullComponents(pmc, 1)) return false;
  std::vector<std::vector<char>> con(graph.n());
  for (int x : pmc) {
    con[x].resize(graph.n());
  }
  for (const auto& nbs : graph.CompNeighs(pmc)) {
    for (int i = 0; i < (int)nbs.size(); i++) {
      for (int ii = i+1; ii < (int)nbs.size(); ii++) {
        int a = std::min(nbs[i], nbs[ii]);
        int b = std::max(nbs[i], nbs[ii]);
        con[a][b]=1;
      }
    }
  }
  for (int i = 0; i < (int)pmc.size(); i++) {
    for (int ii = i+1; ii < (int)pmc.size(); ii++) {
      int a = std::min(pmc[i], pmc[ii]);
      int b = std::max(pmc[i], pmc[ii]);
      if (!con[a][b] && !graph.HasEdge(a, b)) {
        return false;
      }
    }
  }
  return true;
}

std::vector<std::vector<int>> OneMoreVertex(const Graph& new_graph, const std::vector<std::vector<int>>& pmcs, 
  const std::vector<std::vector<int>>& minseps, const std::vector<std::vector<int>>& new_minseps, int x) {
  std::vector<std::vector<int>> new_pmcs;
  for (const auto& pmc : pmcs) {
    if (!new_graph.HasNFullComponents(pmc, 1)) {
      new_pmcs.push_back(pmc);
    } else {
      if (extend2(new_graph, pmc, x)) {
        new_pmcs.push_back(pmc);
        new_pmcs.back().push_back(x);
      }
    }
  }
  int i2 = 0;
  for (const auto& minsep : new_minseps) {
    if (std::find(minsep.begin(), minsep.end(), x) != minsep.end()) continue;
    if (extend2(new_graph, minsep, x)) {
      new_pmcs.push_back(minsep);
      new_pmcs.back().push_back(x);
      if (new_graph.HasNFullComponents(new_pmcs.back(), 1)) {
        new_pmcs.pop_back();
      }
    }
    while (i2 < (int)minseps.size() && minseps[i2] < minsep) i2++;
    if (i2 < (int)minseps.size() && minseps[i2] == minsep) continue;

    std::set<std::vector<int>> tried;
    std::vector<int> npmc = minsep;
    std::vector<char> inc(new_graph.n());
    for (const auto& c : new_graph.Components(minsep)) {
      for (int cv : c) {
        inc[cv] = true;
      }
      for (const auto& minsep2 : new_minseps) {
        std::vector<int> add;
        for (int y : minsep2) {
          if (inc[y]) {
            add.push_back(y);
          }
        }
        if (!add.empty()) {
          if (tried.count(add)) continue;
          tried.insert(add);
          for (int y : add) npmc.push_back(y);
          if (IsPmc(new_graph, npmc)) {
            new_pmcs.push_back(npmc);
          }
          for (int i = 0; i < (int)add.size(); i++) npmc.pop_back();
        }
      }
      for (int cv : c) {
        inc[cv] = false;
      }
    }
  }
  return new_pmcs;
}

std::vector<std::vector<int>> MinSepsRemoved(const Graph& graph, const std::vector<std::vector<int>>& old_minseps, int a) {
  std::vector<std::vector<int>> minseps;
  for (const auto& oms : old_minseps) {
    if (std::binary_search(oms.begin(), oms.end(), a)) {
      std::vector<int> nms;
      for (int v : oms) {
        if (v!=a){
          nms.push_back(v);
        }
      }
      assert(graph.IsMinsep(nms));
      minseps.push_back(nms);
    } else{
      for (auto nbs : graph.CompNeighs(oms)) {
        if (graph.IsMinsep(nbs)) {
          std::sort(nbs.begin(), nbs.end());
          minseps.push_back(nbs);
        }
      }
    }
  }
  utils::SortAndDedup(minseps);
  return minseps;
}

std::vector<std::vector<int>> FindMinSeps(const Graph& graph) {
  std::vector<std::vector<int>> minseps;
  std::set<std::vector<int>> ff;
  std::vector<int> u(graph.n());
  for (int i = 0; i < graph.n(); i++) {
    if (graph.Neighbors(i).empty()) continue;
    auto block = graph.Neighbors(i);
    block.push_back(i);
    for (auto nbs : graph.CompNeighs(block)) {
      std::sort(nbs.begin(), nbs.end());
      if (!ff.count(nbs)) {
        ff.insert(nbs);
        minseps.push_back(nbs);
      }
    }
  }
  for (int i = 0; i < (int)minseps.size(); i++) {
    auto minsep = minseps[i];
    for (int x : minsep) {
      auto block = minsep;
      for (int v : graph.Neighbors(x)) {
        block.push_back(v);
      }
      for (auto nbs : graph.CompNeighs(block)) {
        std::sort(nbs.begin(), nbs.end());
        if (!ff.count(nbs)) {
          ff.insert(nbs);
          minseps.push_back(nbs);
        }
      }
    }
  }
  std::sort(minseps.begin(), minseps.end());
  return minseps;
}
} // namespace

std::vector<std::vector<int>> Pmcs(const Graph& graph) {
  if (graph.n() == 0) return {};
  assert(graph.IsConnectedOrIsolated());
  std::vector<int> order = mcs::Mcs(graph);
  std::reverse(order.begin(), order.end());
  std::vector<int> ord(graph.n());
  for (int i = 0; i < graph.n(); i++) {
    ord[order[i]] = i;
  }

  std::vector<Graph> graphs = {Graph(graph.n())};
  for (int i = 1; i < graph.n(); i++) {
    graphs.push_back(graphs[i-1]);
    int x = order[i];
    for (int nx : graph.Neighbors(x)) {
      if (ord[nx] < ord[x]) {
        graphs[i].AddEdge(nx, x);
      }
    }
  }
  std::vector<std::vector<std::vector<int>>> minseps(graph.n());
  minseps[graph.n() - 1] = FindMinSeps(graphs[graph.n() - 1]);
  Log::Write(3, "Enum minseps ", graph.n(), " ", minseps[graph.n() - 1].size());

  for (int i = graph.n() - 2; i>0; i--) {
    minseps[i] = MinSepsRemoved(graphs[i], minseps[i+1], order[i+1]);
    Log::Write(3, "Enum minseps ", i+1, " ", minseps[i].size());
  }

  std::vector<std::vector<int>> pmcs = {{order[0]}};
  for (int i = 1; i < graph.n(); i++) {
    assert(graphs[i].IsConnectedOrIsolated());
    pmcs = OneMoreVertex(graphs[i], pmcs, minseps[i-1], minseps[i], order[i]);
    Log::Write(3, "Enumerating... ", i, " ", pmcs.size(), " ", minseps[i].size());
    for (auto& pmc : pmcs) {
      std::sort(pmc.begin(), pmc.end());
    }
    utils::SortAndDedup(pmcs);
  }
  return pmcs;
}
} // namespace comb_enumerator
} // namespace triangulator
