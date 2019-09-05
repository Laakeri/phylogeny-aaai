CC = g++
CFLAGS = -DIL_STD -std=c++11 -O3 -flto -Wall -Wextra -Wshadow -g -Wfatal-errors -Wno-sign-compare -Wno-literal-suffix -fPIC -march=native
INC = -I $(CPLEXDIR)/cplex/include -I $(CPLEXDIR)/concert/include
LIB = -lm -lz -lpthread -L $(CPLEXDIR)/cplex/lib/x86-64_linux/static_pic -L $(CPLEXDIR)/concert/lib/x86-64_linux/static_pic -lconcert -lilocplex -lcplex
OBJFILES = ../bt-maxsat/bitset.o ../bt-maxsat/comb_enum.o ../bt-maxsat/graph.o ../bt-maxsat/id_set.o ../bt-maxsat/io.o ../bt-maxsat/matrix.o ../bt-maxsat/mcs.o ../bt-maxsat/phyl_mat.o ../bt-maxsat/phylogenypreprocessor.o ../bt-maxsat/staticset.o ../bt-maxsat/utils.o

all: main

main: main.o $(OBJFILES)
	$(CC) $(INC) $(CFLAGS) main.o $(OBJFILES) -o main $(LIB)

%.o: %.cpp
	$(CC) $(INC) $(CFLAGS) -MMD -c $< -o $@

-include *.d

clean:
	rm -f *.o *.d
	rm -f main

.PHONY: all clean
