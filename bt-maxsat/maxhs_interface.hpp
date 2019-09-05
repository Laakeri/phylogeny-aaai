#pragma once

#include <vector>
#include <memory>

#include "maxsat_interface.hpp"

namespace triangulator {
class MaxhsInterface : public MaxSatInterface {
public:
  Lit NewVar() final;
  void AddClause(std::vector<Lit> clause) final;
  void AddSoftClause(std::vector<Lit> clause, uint64_t weight) final;
  bool SolutionValue(Lit lit) final;
  bool Solve() final;
  MaxhsInterface();
private:
  int vars_;
  std::vector<char> solution_value_;
  std::vector<std::vector<int>> clauses_;
  std::vector<uint64_t> weights_;
};
} // namespace triangulator
