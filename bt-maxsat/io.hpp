#pragma once

#include <istream>

#include "phyl_mat.hpp"

namespace triangulator {

class Io {
public:
  PhylMat ReadPhylMat(std::istream& in);
};
} // namespace triangulator
