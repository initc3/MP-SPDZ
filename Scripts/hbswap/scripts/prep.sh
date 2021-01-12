#!/usr/bin/env bash
set -e

players=4
threshold=1
port=5000
prime=52435875175126190479447740508185965837690552500527637822603658699938581184513

prog="malicious-shamir-party.x"

prepare() {
  Scripts/setup-ssl.sh $players

  pkill -f $prog || true
  pkill -f random-shamir.x || true
  pkill -f start_server.py || true
  pkill -f server.go || true

  rm -rf Persistence
  rm -rf /opt/hbswap/data
  rm -rf /opt/hbswap/log
  rm -rf /opt/hbswap/db
  rm -rf Player-Data/4-MSp-255

  mkdir Persistence
  mkdir /opt/hbswap/data
  mkdir /opt/hbswap/log
  mkdir /opt/hbswap/db
}

compile() {
  ./compile.py -v -C -F 256 $1
}

run() {
    ./$prog -N $players -T $threshold -p 0 -pn $port $1 -P $prime &
    ./$prog -N $players -T $threshold -p 1 -pn $port $1 -P $prime  &
    ./$prog -N $players -T $threshold -p 2 -pn $port $1 -P $prime  &
    ./$prog -N $players -T $threshold -p 3 -pn $port $1 -P $prime
}

org() {
  mv 'Persistence/Transactions-P0.data' '/opt/hbswap/data/Pool-P0.data'
  mv 'Persistence/Transactions-P1.data' '/opt/hbswap/data/Pool-P1.data'
  mv 'Persistence/Transactions-P2.data' '/opt/hbswap/data/Pool-P2.data'
  mv 'Persistence/Transactions-P3.data' '/opt/hbswap/data/Pool-P3.data'
}

prepare

compile hbswap_init
compile hbswap_trade_prep
compile hbswap_trade

run hbswap_init
org