
N=4
batch_size=2000
port=4900
app=tutorial

#./compile.py tutorial
#./Scripts/setup-ssl.sh 4 /opt/ssl/

pkill -f .x

for ((id = 0; id < $N; id++ )) do
  ./mal-shamir-offline.x -N $N -T 1 -p $id -pn $port -ip HOSTS.txt -b $batch_size --prep-dir offline/$app -npfs $app > log_$id.txt 2>&1 &
done
