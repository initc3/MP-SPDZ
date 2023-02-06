set -x

N=$1

pkill -f .x
rm -rf offline/data
mkdir -p offline/data
for ((id = 0; id < $N; id++ )) do
  time cp -rf offline/tutorial offline/data/s$id
done

for ((id = 0; id < $N; id++ )) do
#  ./malicious-shamir-party.x -v -N $N -T 1 -p $id -pn 4900 -F --prep-dir offline/data/s$id -npfs tutorial > log_$id.txt 2>&1 &
  ./malicious-shamir-party.x -v -N $N -T 1 -p $id -pn 4900 -ip HOSTS.txt -F --prep-dir offline/data/s$id -npfs tutorial > log_$id.txt 2>&1 &
done
