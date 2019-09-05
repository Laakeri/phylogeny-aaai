#!/bin/sh
./bt-maxsat/triangulator pp < $1 > tmp/tmp1.phy
./bin_ip/main < tmp/tmp1.phy