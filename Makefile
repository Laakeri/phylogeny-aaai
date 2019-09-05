CPLEXDIR = /home/EXAMPLE/cplex-install
export CPLEXDIR

all: bt-maxsat minsep_ip bin_ip phylgen convert

bt-maxsat:
	$(MAKE) -C bt-maxsat all

msdir/ms: msdir/ms.c msdir/streec.c msdir/rand1.c
	gcc -o msdir/ms msdir/ms.c msdir/streec.c msdir/rand1.c -lm

phylgen: msdir/ms phylgen.cpp
	g++ phylgen.cpp -o phylgen -std=c++11 -O2 -Wall -Wextra -Wshadow -g -Wfatal-errors

convert: convert.cpp
	g++ convert.cpp -o convert -std=c++11 -O2 -Wall -Wextra -Wshadow -g -Wfatal-errors

minsep_ip:
	$(MAKE) -C minsep_ip all

bin_ip:
	$(MAKE) -C bin_ip all

clean:
	$(MAKE) -C bt-maxsat clean
	$(MAKE) -C minsep_ip clean
	$(MAKE) -C bin_ip clean
	rm -f msdir/ms
	rm -f phylgen
	rm -f convert

.PHONY: all bt-maxsat minsep_ip bin_ip clean
