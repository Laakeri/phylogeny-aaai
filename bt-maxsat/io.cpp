#include "io.hpp"

#include <string>
#include <istream>
#include <cassert>

#include "phyl_mat.hpp"
#include "matrix.hpp"

namespace triangulator {
PhylMat Io::ReadPhylMat(std::istream& in) {
  std::ios_base::sync_with_stdio(0);
  std::cin.tie(0);
  int n, m;
  in>>n>>m;
  assert(n > 0 && m > 0);
  Matrix<int> mat(n, m);
  for (int i = 0; i < n; i++) {
    for (int ii = 0; ii < m; ii++) {
      std::string t;
      in>>t;
      if (t == "?") {
        mat[i][ii] = -1;
      } else {
        try {
          mat[i][ii] = stoi(t);
        } catch (const std::exception& e) {
          assert(0);
        }
      }
    }
  }
  PhylMat pm(mat);
  return pm;
}
} // namespace triangulator
