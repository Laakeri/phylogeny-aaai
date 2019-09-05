#pragma once

#include "phyl_mat.hpp"

namespace triangulator {
int GeneralCharRemoveMaxSat(const PhylMat& pm);
PhylMat PhylPreprocess(const PhylMat& pm);
} // namespace triangulator