#!/bin/bash

BLS_PRIME=52435875175126190479447740508185965837690552500527637822603658699938581184513
for i in hbmpc_mimc_test; do
    ./compile.py -v -C -F 256  $i || exit 1
done

Scripts/setup-ssl.sh 4

mkdir Player-Data
echo 14 > Player-Data/Input-P0-0
echo 12 > Player-Data/Input-P1-0
echo 8 > Player-Data/Input-P2-0
echo 0 > Player-Data/Input-P3-0

# progs="./mascot-party.x"
progs="./malicious-shamir-party.x"

for prog in $progs; do
    for i in 0 1 2 3; do
	$prog -N 4 -t 1 -p $i -P $BLS_PRIME hbmpc_mimc_test  & pids[${i}]=$!
    done
    for pid in ${pids[*]}; do
	wait $pid
    done
done
