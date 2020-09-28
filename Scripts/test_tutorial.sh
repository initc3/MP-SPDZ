#!/usr/bin/env bash

while getopts XYC opt; do
    case $opt in
	X) compile_opts=-X
	   dabit=1
	   ;;
	Y) dabit=2
	   ;;
    esac
done

shift $[OPTIND-1]

for i in 0 1; do
    seq 0 3 > Player-Data/Input-P$i-0
done

# clean state
rm Player-Data/*Params*
rm Player-Data/*Secrets*

function test_vm
{
    ulimit -c unlimited
    vm=$1
    shift
    if ! Scripts/$vm.sh tutorial $* | grep 'weighted average: 2.333'; then
	for i in 0 1 2; do
	    echo == Party $i
	    cat logs/tutorial-$i
	done
	exit 1
    fi
}

# big buckets for smallest batches
run_opts="$run_opts -B 5"

for dabit in ${dabit:-0 1 2}; do
    if [[ $dabit = 1 ]]; then
	compile_opts="$compile_opts -X"
    elif [[ $dabit = 2 ]]; then
	compile_opts="$compile_opts -Y"
    fi

    mpspdz-compile -R 64 $compile_opts tutorial

    for i in ring semi2k brain mal-rep-ring ps-rep-ring spdz2k; do
	test_vm $i $run_opts
    done

    mpspdz-compile  $compile_opts tutorial

    for i in rep-field shamir mal-rep-field ps-rep-field mal-shamir hemi semi \
		       soho cowgear mascot; do
	test_vm $i $run_opts
    done

    test_vm chaigear $run_opts -l 3 -c 2
done

mpspdz-compile tutorial

test_vm cowgear $run_opts -T
test_vm chaigear $run_opts -T -l 3 -c 2

if test $skip_binary; then
   exit
fi

mpspdz-compile -B 16  $compile_opts tutorial

for i in replicated mal-rep-bin semi-bin ccd mal-ccd; do
    test_vm $i $run_opts
done

test_vm yao

for i in tinier rep-bmr mal-rep-bmr shamir-bmr mal-shamir-bmr; do
    test_vm $i $run_opts
done
