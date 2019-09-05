CC = g++
CFLAGS = -std=c++11 -O3 -flto -Wall -Wextra -Wshadow -g -Wfatal-errors -Wno-sign-compare -Wno-literal-suffix -fPIC -march=native
OBJFILES = bitset.o bt_gencr_maxsat.o comb_enum.o graph.o id_set.o io.o matrix.o maxhs_interface.o maxsat_interface.o mcs.o phyl_mat.o phylogenypreprocessor.o solver.o staticset.o utils.o

all: triangulator

triangulator: triangulator.o $(OBJFILES)
	$(CC) $(CFLAGS) triangulator.o $(OBJFILES) -o triangulator

%.o: %.cpp
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include *.d

clean:
	rm -f *.o *.d

.PHONY: all clean
