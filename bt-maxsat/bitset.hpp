#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace triangulator {

class Bitset {
 public:
  Bitset();
  explicit Bitset(size_t size);
  void Set(size_t i, bool v);
  bool Get(size_t i) const;
  uint64_t Chunk(size_t i) const;
  uint64_t& Chunk(size_t i);
  size_t Chunks() const;
  std::vector<uint64_t> data_;
};

} // namespace triangulator