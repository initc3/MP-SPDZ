
N=4
T=1
port=4900

batch_size=$1

pkill -f .x

for ((id = 0; id < $N; id++ )) do
  ./random-shares.x -N $N -T $T -i $id -p $port -s $batch_size --prep-dir offline/random-bits > log_$id.txt 2>&1 &
done
