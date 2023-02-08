#!/bin/bash

#./compile.py tutorial
#./Scripts/setup-ssl.sh 4 /opt/ssl/

source ./test/utils.sh

batch_size=$1
prep_dir=offline/$app

for ((id = 0; id < $N; id++ )) do
  ./mal-shamir-offline.x -N $N -T $T -p $id -pn $port -ip $ip_config -b $batch_size -P $prime --prep-dir $prep_dir -npfs $app > logs/log_offline_$id.txt 2>&1 &
done
