#!/bin/bash

source ./test/utils.sh

prep_dir=offline/data

rm -rf $prep_dir || true

for ((id = 0; id < $N; id++ )) do
  src_dir=offline/$app/$N-MSp-$k
  dst_dir=$prep_dir/s$id/$N-MSp-$k
  mkdir -p $dst_dir

#  cp -r $src_dir/* $dst_dir

  cp $src_dir/Params-Data $dst_dir
  cp -r $src_dir/Bits-MSp-P$id $dst_dir/Bits-MSp-P$id
  cp -r $src_dir/Triples-MSp-P$id $dst_dir/Triples-MSp-P$id
done

for ((id = 0; id < $N; id++ )) do
  ./malicious-shamir-party.x -v -N $N -T $T -p $id -pn $port -ip $ip_config -P $prime -F --prep-dir $prep_dir/s$id -npfs $app > logs/log_online_$id.txt 2>&1 &
done
