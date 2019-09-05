#include <vector>
#include <iostream>
#include <cassert>
#include <set>

#include <ilcplex/ilocplex.h>

#include "../bt-maxsat/graph.hpp"
#include "../bt-maxsat/comb_enum.hpp"
#include "../bt-maxsat/io.hpp"
#include "../bt-maxsat/phylogenypreprocessor.hpp"

using namespace triangulator;

bool MinsepsCross(const Graph& graph, const std::vector<int>& sep1, const std::vector<int>& sep2) {
  auto cs1=graph.Components(sep1);
  std::vector<int> c1(graph.n());
  for (int i=0;i<(int)cs1.size();i++){
    for (int v : cs1[i]) {
      c1[v]=i+1;
    }
  }
  int s2c=0;
  for (int v : sep2){
    if (s2c==0){
      s2c=c1[v];
    }else if(c1[v]!=0) {
      if (c1[v] != s2c){
        return true;
      }
    }
  }
  return false;
}

void Construct(const Graph& graph, const std::vector<int>& colors, IloEnv& env, IloNumVarArray& char_vars, IloModel& model) {
  auto minseps = comb_enumerator::FindMinSeps(graph);
  IloNumVarArray ms_vars(env);
  for (int i=0;i<(int)minseps.size();i++){
    ms_vars.add(IloNumVar(env, 0, 1, ILOBOOL));
    std::set<int> badcols;
    for (int v:minseps[i]){
      for (int u:minseps[i]){
        if (v!=u && colors[v] == colors[u]){
          badcols.insert(colors[v]);
        }
      }
    }
    for (int c:badcols){
      model.add(ms_vars[i] <= char_vars[c]);
    }
  }

  std::vector<std::vector<int>> cross(minseps.size());
  int crosses=0;
  for (int i=0;i<(int)minseps.size();i++){
    for (int ii=i+1;ii<(int)minseps.size();ii++){
      if (MinsepsCross(graph, minseps[i], minseps[ii])) {
        cross[i].push_back(ii);
        cross[ii].push_back(i);
        crosses++;
      }
    }
  }


  for (int i=0;i<(int)minseps.size();i++){
    for (int ii:cross[i]){
      model.add(ms_vars[i] + ms_vars[ii] <= 1);
    }
    IloExpr ex(env);
    ex += ms_vars[i];
    for (int ii:cross[i]){
      ex += ms_vars[ii];
    }
    model.add(ex >= 1);
  }
}

void Construct2(const Graph& graph, const std::vector<int>& colors, IloEnv& env, IloModel& model) {
  auto minseps = comb_enumerator::FindMinSeps(graph);
  std::vector<std::vector<int>> legal_minseps;
  std::vector<std::vector<int>> illegal_minseps;
  IloNumVarArray ms_vars(env);
  for (auto ms : minseps) {
    bool f=false;
    for (int v : ms){
      for (int u : ms) {
        if (v!=u && colors[v] == colors[u]) {
          f=true;
          break;
        }
      }
      if (f) break;
    }
    if (!f){
      legal_minseps.push_back(ms);
      ms_vars.add(IloNumVar(env, 0, 1, ILOBOOL));
    } else{
      illegal_minseps.push_back(ms);
    }
  }

  std::vector<std::vector<int>> cross(legal_minseps.size());
  int crosses=0;
  for (int i=0;i<(int)legal_minseps.size();i++){
    for (int ii=i+1;ii<(int)legal_minseps.size();ii++){
      if (MinsepsCross(graph, legal_minseps[i], legal_minseps[ii])) {
        cross[i].push_back(ii);
        cross[ii].push_back(i);
        crosses++;
      }
    }
  }
  for (int i=0;i<(int)legal_minseps.size();i++){
    for (int ii:cross[i]){
      model.add(ms_vars[i] + ms_vars[ii] <= 1);
    }
    IloExpr ex(env);
    ex += ms_vars[i];
    for (int ii:cross[i]){
      ex += ms_vars[ii];
    }
    model.add(ex >= 1);
  }
  for (auto ims : illegal_minseps) {
    IloExpr ex(env);
    for (int i=0;i<(int)legal_minseps.size();i++){
      if (MinsepsCross(graph, ims, legal_minseps[i])){
        ex += ms_vars[i];
      }
    }
    model.add(ex >= 1);
  }
}

int main(){
  Io io;
  PhylMat pm = io.ReadPhylMat(std::cin);

  IloEnv env;
  IloModel model(env);
  IloCplex cplex(model);
  cplex.setParam(IloCplex::Threads, 1);

  IloNumVarArray char_vars(env);
  IloExpr obj(env);
  for (int i=0;i<pm.m();i++){
    char_vars.add(IloNumVar(env, 0, 1, ILOBOOL));
    obj += char_vars[i];
  }
  model.add(IloMinimize(env, obj));

  PhylogenyPreprocessor ppp;
  auto instances = ppp.Preprocess(pm);
  std::cout<<instances.size()<<std::endl;
  for (const std::pair<Graph, std::vector<int>>& instance : instances) {
    Construct(instance.first, instance.second, env, char_vars, model);
  }

  cplex.solve();
  assert(cplex.getStatus() == IloAlgorithm::Optimal);
  IloNumArray vals(env);
  cplex.getValues(vals, char_vars);

  std::vector<int> sol;

  for (int i=0;i<pm.m();i++){
    if (vals[i] > 0.5){
      sol.push_back(i);
    }
  }
  env.end();
  std::cout<<"remc "<<sol.size()<<std::endl;
  for (int x:sol){
    std::cout<<x<<" ";
  }
  std::cout<<std::endl;
}