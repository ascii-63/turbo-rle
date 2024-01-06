./net-master-sender 10.140.0.3 10000 .part0 &
./net-master-sender 10.140.0.4 10001 .part1 &
./net-master-sender 10.140.0.5 10002 .part2 &

./net-master-receiver 20000 files/temp/part0 &
./net-master-receiver 20001 files/temp/part1 &
./net-master-receiver 20002 files/temp/part2 &

./master files/enwik9.txt