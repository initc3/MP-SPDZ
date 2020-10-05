#!/bin/bash

# Parameters
BLS_PRIME=52435875175126190479447740508185965837690552500527637822603658699938581184513
players=4
threshold=1

# Compile the programs
for i in hbmpc_mimc_test; do
    ./compile.py -v -C -F 256  $i || exit 1
done

# Setup the network authentication
Scripts/setup-ssl.sh 4

# Set up the player inputs
mkdir -p Player-Data
echo 14 > Player-Data/Input-P0-0
echo 12 > Player-Data/Input-P1-0
echo 8 > Player-Data/Input-P2-0
echo 0 > Player-Data/Input-P3-0


## SPDZ style, n-of-n additive encoding
# Setup online
echo "Running Fake Offline Phase"
Scripts/setup-online.sh $players 256 128 10000 $BLS_PRIME
#Scripts/setup-online.sh $players 256 128
# Run the Online
echo "SPDZ-style Online Phase"
for i in 0 1 2 3; do
    ./Player-Online.x -N 4 -p $i -P $BLS_PRIME hbmpc_mimc_test  & pids[${i}]=$!
done
for pid in ${pids[*]}; do
    wait $pid
done

progs="./malicious-shamir-party.x"
# Run online
for prog in $progs; do
    echo "Combined Offline/Online Experiment $prog"
    for i in 0 1 2 3; do
	$prog -N 4 -T 1 -p $i -P $BLS_PRIME hbmpc_mimc_test  & pids[${i}]=$!
    done
    for pid in ${pids[*]}; do
	wait $pid
    done
done
