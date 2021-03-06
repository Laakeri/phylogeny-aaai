Note: An improved implementation is available in [https://github.com/Laakeri/triangulator-msc](https://github.com/Laakeri/triangulator-msc).


This folder contains the necessary code and test data to reproduce the 
experiments in AAAI 2020 paper, "Finding Most Compatible Phylogenetic 
Trees over Multi-State Characters".

The instance specific experiment results are in results/ directory.

Installing:

1. You need to have CPLEX installed. It is the only component not included here.

2. Compile MaxHS. Follow the instructions in MaxHS-3.0/README. We assume that 
the MaxHS binary is located in the default location 
MaxHS-3.0/build/release/bin/maxhs after this.

3. Edit CPLEXDIR parameter in the first line of the Makefile in this directory.
It should point to the root directory of CPLEX install, i.e. the directory
that contains 'concert' and 'cplex' directories.
Then use 'make' to compile all binaries.



Usage:

Example usage of BT-MaxSAT to compute maximum compatibility:

./bt-maxsat/triangulator maxcomp < instances/mammals.phy

You can replace mammals.phy with other instances in the instances directory.
The instances with prefixes ms_exp1_n, ms_exp1_k, ms_exp1_r are used in the 
first set of experiments and the instances with prefix ms_exp2 are used in the 
second set of experiments. alcataenia, chinese, indo, indo-pp and mammals are 
the real-world benchmarks.


Example usage of PBO to compute maximum compatibility:

./pbo.sh instances/mammals.phy


Example usage of Minsep IP to compute maximum compatibility:

./minsep_ip/main < instances/mammals.phy


Example usage of Bin IP to compute maximum compatibility:

./bin_ip.sh instances/mammals.phy
