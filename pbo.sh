#!/bin/sh
./bt-maxsat/triangulator pp < $1 > tmp/tmp1.phy
./convert toraw < tmp/tmp1.phy > tmp/tmp2.raw
./pbo/translate_pbo_rr < tmp/tmp2.raw > tmp/tmp3.pbo
./pbo/minisatp tmp/tmp3.pbo