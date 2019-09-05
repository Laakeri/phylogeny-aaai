#pragma once

#include <vector>
#include <cstdint>
#include <limits>

namespace triangulator {

class Lit {
public:
  Lit();
  bool IsDef() const;
  bool IsTrue() const;
  bool IsFalse() const;
  static Lit TrueLit();
  static Lit FalseLit();
  Lit operator-() const;
  bool operator==(const Lit& rhs) const;

  Lit(const Lit& rhs) = default;
  Lit& operator=(const Lit& rhs) = default;
private:
  int value_;
  static constexpr int kTrueVal = std::numeric_limits<int>::max();

  Lit(int value);
  int Value() const;
  
  friend class MaxSatInterface;
  friend class SatHelper;
};

class MaxSatInterface {
public:
  static const uint64_t kTopWeight;
  virtual Lit NewVar() = 0;
  virtual void AddClause(std::vector<Lit> clause) = 0;
  virtual void AddSoftClause(std::vector<Lit> clause, uint64_t weight) = 0;
  virtual bool SolutionValue(Lit lit) = 0;
  virtual bool Solve() = 0;

  MaxSatInterface() {}
  virtual ~MaxSatInterface();

  MaxSatInterface(const MaxSatInterface&) = delete;
  MaxSatInterface& operator=(const MaxSatInterface&) = delete;

protected:
  static Lit CreateLit(int value);
  static int LitValue(Lit lit);
};

class SatHelper {
public:
  // Processes false and true literals and tautologies. If contains true, { TrueLit } is returned.
  // Also asserts IsDef()
  static std::vector<Lit> ProcessClause(const std::vector<Lit>& clause);
  SatHelper() = delete;
  SatHelper(const SatHelper&) = delete;
  SatHelper& operator=(const SatHelper&) = delete;
};
} // namespace triangulator
