#!/bin/bash

source ./test/utils.sh

batch_size=$1

prep_dir=offline/buffer

rm -rf $prep_dir/$N-MSp-$k/Bits-*

for ((id = 0; id < $N; id++)) do
  ./random-bits.x -N $N -T $T -i $id -p $port -s $batch_size --prep-dir $prep_dir > logs/log_bits_$id.txt 2>&1 &
done
