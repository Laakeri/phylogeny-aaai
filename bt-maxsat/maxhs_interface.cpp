#include "maxhs_interface.hpp"

#include <vector>
#include <cassert>
#include <fstream>
#include <sstream>

#include "maxsat_interface.hpp"
#include "utils.hpp"

namespace triangulator {

namespace {
std::string TmpInstance(int a, int b, int c) {
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  uint64_t micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
  return "tmp/instance"+std::to_string(micros)+"_"+std::to_string(a)+"_"+std::to_string(b)+"_"+std::to_string(c)+".wcnf";
}
} // namespace

MaxhsInterface::MaxhsInterface()
  : vars_(0) { }

Lit MaxhsInterface::NewVar() {
  vars_++;
  return CreateLit(vars_);
}
void MaxhsInterface::AddClause(std::vector<Lit> clause) {
  clause = SatHelper::ProcessClause(clause);
  if (clause.size() == 1 && clause[0].IsTrue()) return;
  std::vector<int> tclause(clause.size());
  for (int i = 0; i < (int)clause.size(); i++) {
    tclause[i] = LitValue(clause[i]);
  }
  clauses_.push_back(tclause);
  weights_.push_back(kTopWeight);
}
void MaxhsInterface::AddSoftClause(std::vector<Lit> clause, uint64_t weight) {
  clause = SatHelper::ProcessClause(clause);
  if (clause.size() == 1 && clause[0].IsTrue()) return;
  std::vector<int> tclause(clause.size());
  for (int i = 0; i < (int)clause.size(); i++) {
    tclause[i] = LitValue(clause[i]);
  }
  clauses_.push_back(tclause);
  weights_.push_back(weight);
}
bool MaxhsInterface::SolutionValue(Lit lit) {
  assert(lit.IsDef());
  if (lit.IsTrue()) return true;
  if (lit.IsFalse()) return false;
  int lit_value = LitValue(lit);
  assert(lit_value != 0 && abs(lit_value) <= vars_);
  bool solution_value = solution_value_[abs(lit_value) - 1];
  if (lit_value > 0) {
    return solution_value;
  } else {
    return !solution_value;
  }
}
bool MaxhsInterface::Solve() {
  int maxvar = 1;
  uint64_t softsum = 1;
  for (uint64_t w : weights_) {
    if (w < kTopWeight) {
      softsum += w;
    }
  }
  for (const auto& c : clauses_) {
    for (int t : c) {
      maxvar = std::max(std::abs(t), maxvar);
    }
  }
  std::string tmp_file_in = TmpInstance(clauses_.size(), maxvar, 0);
  std::string tmp_file_out = TmpInstance(clauses_.size(), maxvar, 1);
  std::ofstream out(tmp_file_in);
  out<<"p wcnf "<<maxvar<<" "<<clauses_.size()<<" "<<softsum<<'\n';
  for (int i = 0; i < (int)clauses_.size(); i++) {
    if (weights_[i] == kTopWeight) out<<softsum;
    else out<<weights_[i];
    for (int t : clauses_[i]) {
      out<<" "<<t;
    }
    out<<" 0\n";
  }
  out<<std::endl;
  out.close();
  utils::DisableStdout();
  assert(0 == system(("./MaxHS-3.0/build/release/bin/maxhs " + tmp_file_in + " > " + tmp_file_out).c_str()));
  utils::ReEnableStdout();
  std::ifstream in(tmp_file_out);
  std::vector<int> out_solution;
  std::string tmpline;
  bool solved = false;
  while (getline(in, tmpline)) {
    if ((int)tmpline.size() > 2 && tmpline[0] == 'v' && tmpline[1] == ' ') {
      std::stringstream ss;
      ss<<tmpline;
      std::string t;
      ss>>t;
      assert(t == "v");
      int x;
      while (ss>>x){
        assert(x != 0);
        out_solution.push_back(x);
      }
    }
    if ((int)tmpline.size() > 2 && tmpline[0] == 's' && tmpline[1] == ' ') {
      std::stringstream ss;
      ss<<tmpline;
      std::string t;
      ss>>t;
      assert(t == "s");
      std::string t1,t2;
      ss>>t1>>t2;
      if (t1=="OPTIMUM" && t2 == "FOUND") {
        solved = true;
      }
    }
  }
  in.close();
  assert(0 == system(("rm -f " + tmp_file_out).c_str()));
  assert(0 == system(("rm -f " + tmp_file_in).c_str()));
  for (int x : out_solution) {
    assert(x != 0);
    if (abs(x) > (int)solution_value_.size()) solution_value_.resize(abs(x));
    if (x > 0) solution_value_[abs(x) - 1] = true;
    else solution_value_[abs(x) - 1] = false;
  }
  return solved;
}
} // namespace triangulator
