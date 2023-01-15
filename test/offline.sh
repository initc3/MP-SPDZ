
N=$1

#./compile.py tutorial
#./Scripts/setup-ssl.sh $N
#cp -r Player-Data/* /opt/ssl/

pkill -f .x

for ((id = 0; id < $N; id++ )) do
  ./mal-shamir-offline.x -N $N -T 1 -p $id -pn 4900 --prep-dir offline/tutorial -npfs tutorial > log_$id.txt 2>&1 &
done
