#include <iostream>
#include <iomanip>

#include "utils.hpp"
#include "io.hpp"
#include "solver.hpp"
#include "phyl_mat.hpp"

int main(int argc, char** argv) {
  const std::string wrong_arg = "The first argument should be either 'maxcomp' for computing maximum compatibility or 'pp' for preprocessing.";
  if (argc < 2) triangulator::utils::ErrorDie(wrong_arg);
  std::string farg(argv[1]);
  triangulator::Log::SetLogLevel(3);
  if (farg == "maxcomp") {
    triangulator::Io io;
    triangulator::PhylMat pm = io.ReadPhylMat(std::cin);
    int sol = triangulator::GeneralCharRemoveMaxSat(pm);
    std::cout << sol << std::endl;
  } else if (farg == "pp") {
    triangulator::Io io;
    triangulator::PhylMat pm = io.ReadPhylMat(std::cin);
    triangulator::PhylMat pp_pm = triangulator::PhylPreprocess(pm);
    std::cout << pp_pm.n() << " " << pp_pm.m() << std::endl;
    for (int i = 0; i < pp_pm.n(); i++) {
      for (int ii = 0; ii < pp_pm.m(); ii++) {
        if (pp_pm.GetMat(i, ii) == -1) {
          std::cout<<"? ";
        } else {
          std::cout<<pp_pm.GetMat(i, ii)<<" ";
        }
      }
      std::cout << std::endl;
    }
  } else {
    triangulator::utils::ErrorDie(wrong_arg);
  }
}
