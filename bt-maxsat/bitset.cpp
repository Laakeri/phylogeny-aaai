#include "bitset.hpp"

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace triangulator {

Bitset::Bitset(size_t size) : data_((size + 63) / 64) {}

void Bitset::Set(size_t i, bool v) {
  if (v) {
    data_[i/64] |= ((uint64_t)1 << (uint64_t)(i%64));
  } else {
    data_[i/64] &= (~((uint64_t)1 << (uint64_t)(i%64)));
  }
}

bool Bitset::Get(size_t i) const {
  return data_[i/64] & ((uint64_t)1 << (uint64_t)(i%64));
}

uint64_t Bitset::Chunk(size_t i) const {
  return data_[i];
}

uint64_t& Bitset::Chunk(size_t i) {
  return data_[i];
}

Bitset::Bitset() : Bitset(0) {}

size_t Bitset::Chunks() const {
  return data_.size();
}

} // namespace triangulator